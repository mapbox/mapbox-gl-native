#!/usr/bin/env node

const jwt = require('jsonwebtoken');
const github = require('@octokit/rest').plugin(require('@octokit/plugin-retry'))({
    retry: { doNotRetry: [ /* Empty — retry on any error code. */ ] }
})
const zlib = require('zlib');
const AWS = require('aws-sdk');
const fs = require('fs');

const SIZE_CHECK_APP_ID = 14028;
const SIZE_CHECK_APP_INSTALLATION_ID = 229425;

var coverage;

try {
    coverage = JSON.parse(fs.readFileSync(process.argv[2]));
    if (typeof coverage.documented !== 'number' || typeof coverage.total !== 'number' ||
        coverage.documented > coverage.total) {
        throw new Error('Invalid coverage.json file');
    }
} catch (error) {
    console.log('Failed to parse json file with coverage data: ', error);
    process.exit(1);
}

process.on('unhandledRejection', error => {
    console.log(error);
    process.exit(1);
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
        const percentage = coverage.documented * 100 / coverage.total;
        const title = `${percentage.toFixed(2)}%`;
        const date = new Date().toISOString().substring(0, 19);

        var promises = [github.checks.create({
            owner: 'mapbox',
            repo: 'mapbox-gl-native',
            name: 'Doxygen coverage',
            head_branch: process.env['CIRCLE_BRANCH'],
            head_sha: process.env['CIRCLE_SHA1'],
            status: 'completed',
            conclusion: 'success',
            completed_at: new Date().toISOString(),
            output: {
                title,
                summary: `There is doxygen documentation for ${percentage}% of the public symbols (${coverage.documented} out of ${coverage.total})`
            }
        })];

        if (process.env['CIRCLE_BRANCH'] === 'master') {
            promises.push(new AWS.S3({region: 'us-east-1'}).putObject({
                Body: zlib.gzipSync(JSON.stringify({
                    'created_at': date,
                    'documented': coverage.documented,
                    'total': coverage.total,
                    'commit': process.env['CIRCLE_SHA1']
                })),
                Bucket: 'mapbox-loading-dock',
                Key: `raw/mobile_staging.docs_coverage/${date.substring(0,10)}/${process.env['CIRCLE_SHA1']}.json.gz`,
                CacheControl: 'max-age=300',
                ContentEncoding: 'gzip',
                ContentType: 'application/json'
            }).promise());
        }

        return Promise.all(promises).then(data => {
            return console.log('Succesfully uploaded doxygen coverage metrics');
        }).catch(err => {
            console.log('Error uploading doxygen coverage metrics: ' + err.message);
            return err;
        });
    });
