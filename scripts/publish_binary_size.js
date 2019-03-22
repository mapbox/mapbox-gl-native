#!/usr/bin/env node

const jwt = require('jsonwebtoken');
const github = require('@octokit/rest').plugin(require('@octokit/plugin-retry'))({
    retry: { doNotRetry: [ /* Empty — retry on any error code. */ ] }
})
const zlib = require('zlib');
const AWS = require('aws-sdk');

const SIZE_CHECK_APP_ID = 14028;
const SIZE_CHECK_APP_INSTALLATION_ID = 229425;

process.on('unhandledRejection', error => {
    console.log(error);
    process.exit(1)
});

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

// Must be in sync with the definition in metrics/binary-size/index.html on the gh-pages branch.
const platforms = [
    { 'platform': 'iOS', 'arch': 'armv7' },
    { 'platform': 'iOS', 'arch': 'arm64' },
    { 'platform': 'iOS', 'arch': 'Dynamic' },
    { 'platform': 'Android', 'arch': 'arm-v7' },
    { 'platform': 'Android', 'arch': 'arm-v8' },
    { 'platform': 'Android', 'arch': 'x86' },
    { 'platform': 'Android', 'arch': 'x86_64' }
];

const sizeCheckInfo = [];
const rows = [];
const date = new Date().toISOString().substring(0, 19);

function query(after) {
    return github.request({
        method: 'POST',
        url: '/graphql',
        headers: {
            // https://developer.github.com/changes/2018-07-11-graphql-checks-preview/
            accept: 'application/vnd.github.antiope-preview'
        },
        query: `query {
              repository(owner: "mapbox", name: "mapbox-gl-native") {
                ref(qualifiedName: "master") {
                  target {
                    ... on Commit {
                      history(first: 100, before: "36c6a8ea79bbd2596abb58ffb58debf65a4ea13d" ${after ? `, after: "${after}"` : ''}) {
                        pageInfo {
                          hasNextPage
                          endCursor
                        }
                        edges {
                          node {
                            oid
                            messageHeadline
                            checkSuites(first: 1, filterBy: {appId: ${SIZE_CHECK_APP_ID}}) {
                              nodes {
                                checkRuns(first: 10) {
                                  nodes {
                                    name
                                    conclusion
                                    title
                                    summary
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }`
    }).then((result) => {
        
        const history = result.data.data.repository.ref.target.history;

        for (const edge of history.edges) {
            const commit = edge.node;
            const suite = commit.checkSuites.nodes[0];

            if (!suite)
                continue;
            const allRuns = commit.checkSuites.nodes[0].checkRuns.nodes;
            
            const sizeCheckRuns = allRuns.filter(function (run) {
              return run.name.match(/Size - (\w+) ([\w-]+)/);
            });
            
            const row = [`${commit.oid.slice(0, 7)} - ${commit.messageHeadline}`];

            for (let i = 0; i < platforms.length; i++) {
                const {platform, arch} = platforms[i];

                const run = sizeCheckRuns.find((run) => {
                    const [, p, a] = run.name.match(/Size - (\w+) ([\w-]+)/);
                    return platform === p && arch === a;
                });
                
                row[i + 1] = run ? +run.summary.match(/is (\d+) bytes/)[1] : undefined;
            }
            rows.push(row);
        }

        if (history.pageInfo.hasNextPage) {
            return query(history.pageInfo.endCursor);
        } else {
          const latestRun = rows[0]
          const runSizeMeasurements = latestRun.slice(1)

          for (let i = 0; i < platforms.length; i++) {
              const {platform, arch} = platforms[i];

              sizeCheckInfo.push(JSON.stringify({
                  'sdk': 'maps',
                  'platform' : platform,
                  'arch': arch,
                  'size' : runSizeMeasurements[i],
                  'created_at': date
              }));
          }
        }
    });
}

github.apps.createInstallationToken({installation_id: SIZE_CHECK_APP_INSTALLATION_ID})
    .then(({data}) => {
      github.authenticate({type: 'token', token: data.token});

      return query().then(function() {
        // Uploads to data source used by 
        // http://mapbox.github.io/mapbox-gl-native/metrics/binary-size/
        var firstPromise = new AWS.S3({region: 'us-east-1'}).putObject({
            Body: zlib.gzipSync(JSON.stringify(rows.reverse())),
            Bucket: 'mapbox',
            Key: 'mapbox-gl-native/metrics/binary-size/data.json',
            ACL: 'public-read',
            CacheControl: 'max-age=300',
            ContentEncoding: 'gzip',
            ContentType: 'application/json'
        }).promise();
        // Uploads to data source used by Mapbox internal metrics dashboards
        var secondPromise = new AWS.S3({region: 'us-east-1'}).putObject({
            Body: zlib.gzipSync(sizeCheckInfo.join('\n')),
            Bucket: 'mapbox-loading-dock',
            Key: `raw/mobile.binarysize/${date.substring(0,10)}/mapbox-maps-ios-android-${process.env['CIRCLE_SHA1']}.json.gz`,
            CacheControl: 'max-age=300',
            ContentEncoding: 'gzip',
            ContentType: 'application/json'
        }).promise();
        
        return Promise.all([firstPromise, secondPromise]).then(data => {
          return console.log("Successfully uploaded all binary size metrics to S3");
        }).catch(err => {
          console.log("Error uploading binary size metrics to S3 " + err.message);
          return err;
        });
      });
});

  