#!/usr/bin/env node

const jwt = require('jsonwebtoken');
const github = require('@octokit/rest')();
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

github.apps.createInstallationToken({installation_id: SIZE_CHECK_APP_INSTALLATION_ID})
    .then(({data}) => {
        github.authenticate({type: 'token', token: data.token});
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
                      history(first: 100, since: "2018-07-01T00:00:00Z") {
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
            // Must be in sync with the definition in metrics/binary-size/index.html on the gh-pages branch.
            const platforms = [
                { 'platform': 'iOS', 'arch': 'armv7' },
                { 'platform': 'iOS', 'arch': 'arm64' },
                { 'platform': 'Android', 'arch': 'arm-v7' },
                { 'platform': 'Android', 'arch': 'arm-v8' },
                { 'platform': 'Android', 'arch': 'x86' },
                { 'platform': 'Android', 'arch': 'x86_64' }
            ];

            const commits = result.data.data.repository.ref.target.history.edges.reverse();
            const rows = [];

            for (const edge of commits) {
                const commit = edge.node;
                const suite = commit.checkSuites.nodes[0];

                if (!suite)
                    continue;

                const runs = commit.checkSuites.nodes[0].checkRuns.nodes;
                const row = [`${commit.oid.slice(0, 7)} - ${commit.messageHeadline}`];

                for (let i = 0; i < platforms.length; i++) {
                    const {platform, arch} = platforms[i];

                    const run = runs.find((run) => {
                        const [, p, a] = run.name.match(/Size - (\w+) ([\w-]+)/);
                        return platform === p && arch === a;
                    });

                    row[i + 1] = run ? +run.summary.match(/is (\d+) bytes/)[1] : undefined;
                }

                rows.push(row);
            }

            return new AWS.S3({region: 'us-east-1'}).putObject({
                Body: zlib.gzipSync(JSON.stringify(rows)),
                Bucket: 'mapbox',
                Key: 'mapbox-gl-native/metrics/binary-size/data.json',
                ACL: 'public-read',
                CacheControl: 'max-age=300',
                ContentEncoding: 'gzip',
                ContentType: 'application/json'
            }).promise();
        });
    });
