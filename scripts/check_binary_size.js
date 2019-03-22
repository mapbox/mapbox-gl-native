#!/usr/bin/env node

const jwt = require('jsonwebtoken');
const github = require('@octokit/rest').plugin(require('@octokit/plugin-retry'))({
    retry: { doNotRetry: [ /* Empty — retry on any error code. */ ] }
})
const prettyBytes = require('pretty-bytes');
const fs = require('fs');

const SIZE_CHECK_APP_ID = 14028;
const SIZE_CHECK_APP_INSTALLATION_ID = 229425;

const file = process.argv[2];
const label = process.argv[3];
const {size} = fs.statSync(file);
const name = `Size - ${label}`;

process.on('unhandledRejection', error => {
    console.log(error);
    process.exit(1)
});

const pk = process.env['SIZE_CHECK_APP_PRIVATE_KEY'];
if (!pk) {
    console.log('Fork PR; not computing size.');
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

function getPriorSize() {
    const mergeBase = process.env['CIRCLE_MERGE_BASE'];
    if (!mergeBase) {
        console.log('No merge base available.');
        return Promise.resolve(null);
    }

    return github.checks.listForRef({
        owner: 'mapbox',
        repo: 'mapbox-gl-native',
        ref: mergeBase
    }).then(({data}) => {
        const run = data.check_runs.find(run => run.name === name);
        if (!run) {
            console.log('No matching check found.');
            return Promise.resolve(null);
        }
        return +run.output.summary.match(/`.*` is (\d+) bytes/)[1];
    });
}

github.apps.createInstallationToken({installation_id: SIZE_CHECK_APP_INSTALLATION_ID})
    .then(({data}) => {
        github.authenticate({type: 'token', token: data.token});
        getPriorSize().then(prior => {
            const title = (() => {
                if (prior) {
                    const change = size - prior;
                    const percent = (change / prior) * 100;
                    return `${change >= 0 ? '+' : ''}${prettyBytes(change)} ${percent.toFixed(3)}% (${prettyBytes(size)})`;
                } else {
                    return prettyBytes(size);
                }
            })();

            console.log(`${label}: ${title} (${size} bytes)`);

            return github.checks.create({
                owner: 'mapbox',
                repo: 'mapbox-gl-native',
                name: `Size - ${label}`,
                head_branch: process.env['CIRCLE_BRANCH'],
                head_sha: process.env['CIRCLE_SHA1'],
                status: 'completed',
                conclusion: 'success',
                completed_at: new Date().toISOString(),
                output: {
                    title,
                    summary: `\`${file}\` is ${size} bytes (${prettyBytes(size)})`
                }
            });
        })
    });
