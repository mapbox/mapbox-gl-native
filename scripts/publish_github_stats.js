#!/usr/bin/env node

const assert = require('assert');
const jwt = require('jsonwebtoken');
const github = require('@octokit/rest')();
const zlib = require('zlib');
const AWS = require('aws-sdk');

const SIZE_CHECK_APP_ID = 14028;
const SIZE_CHECK_APP_INSTALLATION_ID = 229425;

// Error handling

process.on('unhandledRejection', error => {
  console.log(error);
  process.exit(1)
});

// Github authorization

const pk = process.env['SIZE_CHECK_APP_PRIVATE_KEY'];
if (!pk) {
  console.log('Fork PR; not publishing size.');
  process.exit(0);
}

const key = Buffer.from(pk, 'base64').toString('binary');
const payload = {
  exp: Math.floor(Date.now() / 1000) + 60,
  iat: Math.floor(Date.now() / 1000),
  iss: SIZE_CHECK_APP_ID
};

const token = jwt.sign(payload, key, {algorithm: 'RS256'});
github.authenticate({type: 'app', token});

// Metrics: Github statistics
let openIssuesTotal = 0;
let openIssuesTotalFromNonMembers = 0;
let openIssuesTotalCore = 0;
let openIssuesTotalAndroid = 0;
let openIssuesTotalIOS = 0;
let openIssuesTotalGLJSParity = 0;
let openPullRequestsTotal = 0;
let openPullRequestsTotalFromNonMembers = 0;
let openPullRequestsSinceLastMonth = 0;
let openPullRequestsSinceLastMonthFromNonMembers = 0;

function collectMetricsFromIssues(issues) {
  const oneMonthAgo = function() { let date = new Date(); date.setMonth(date.getMonth() - 1); return date; }();

  // Metrics
  issues.data.forEach(function (issue) {
    const issueCreatedAt = new Date(issue.created_at);
    const isMapboxAuthor = issue.author_association === "MEMBER";

    if (issue.pull_request) {
      openPullRequestsTotal++;
      if (!isMapboxAuthor) {
        openPullRequestsTotalFromNonMembers++;
      }
      if (issueCreatedAt >= oneMonthAgo) {
        openPullRequestsSinceLastMonth++;
        if (!isMapboxAuthor) {
          openPullRequestsSinceLastMonthFromNonMembers++;
        }
      }
    } else {
      openIssuesTotal++;
      if (!isMapboxAuthor) {
        openIssuesTotalFromNonMembers++;
      }
      issue.labels.forEach(function (label) {
        switch (label.name) {
          case "Core":
            openIssuesTotalCore++;
            break;
          case "Android":
            openIssuesTotalAndroid++;
            break;
          case "iOS":
            openIssuesTotalIOS++;
            break;
          case "GL JS parity":
            openIssuesTotalGLJSParity++;
            break;
          default:
            break;
        }
      });
    }
  });
}

function publishMetrics() {
  let metrics = {
    'created_at': new Date().toISOString().substring(0, 10),
    'open_issues_total': openIssuesTotal,
    'open_issues_total_from_non_members': openIssuesTotalFromNonMembers,
    'open_issues_total_core': openIssuesTotalCore,
    'open_issues_total_android': openIssuesTotalAndroid,
    'open_issues_total_ios': openIssuesTotalIOS,
    'open_issues_total_gl_js_parity': openIssuesTotalGLJSParity,
    'open_pull_requests_total': openPullRequestsTotal,
    'open_pull_requests_total_from_non_members': openPullRequestsTotalFromNonMembers,
    'open_pull_requests_since_last_month': openPullRequestsSinceLastMonth,
    'open_pull_requests_since_last_month_from_non_members': openPullRequestsSinceLastMonthFromNonMembers
  };

  var promise = new AWS.S3({region: 'us-east-1'}).putObject({
    Body: zlib.gzipSync(JSON.stringify(metrics)),
    Bucket: 'mapbox-loading-dock',
    Key: `raw/mobile_staging.github_stats/${metrics['created_at']}/METRIC.json.gz`,
    CacheControl: 'max-age=300',
    ContentEncoding: 'gzip',
    ContentType: 'application/json'
  }).promise();

  return Promise.all([promise]).then(data => {
    return console.log("Successfully uploaded Github Stats metrics to S3");
  }).catch(err => {
    console.log("Error uploading Github Stats metrics to S3 " + err.message);
    return err;
  });
}

function recursiveListForRepo(query) {
  assert(query);
  query.then(result => {
    collectMetricsFromIssues(result);
    if (github.hasNextPage(result)) {
      recursiveListForRepo(github.getNextPage(result));
    } else {
      publishMetrics();
    }
  }).catch(error => {
    console.log("Error fetching the repository issues list: " + err.message);
  });
}

github.apps.createInstallationToken({ installation_id: SIZE_CHECK_APP_INSTALLATION_ID })
  .then(({data}) => {
    github.authenticate({ type: 'token', token: data.token });
  })
  .then(() => {
    recursiveListForRepo(github.issues.listForRepo({ owner: 'mapbox', repo: 'mapbox-gl-native', state: 'open', per_page: 100 }));
  });
