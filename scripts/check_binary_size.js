#!/usr/bin/env node

const jwt = require('jsonwebtoken');
const github = require('@octokit/rest')();
const prettyBytes = require('pretty-bytes');
const fs = require('fs');
const {execSync} = require('child_process');

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

const key = Buffer.from(process.env['SIZE_CHECK_APP_PRIVATE_KEY'], 'base64').toString('binary');
const payload = {
    exp: Math.floor(Date.now() / 1000) + 60,
    iat: Math.floor(Date.now() / 1000),
    iss: SIZE_CHECK_APP_ID
};

const token = jwt.sign(payload, key, {algorithm: 'RS256'});
github.authenticate({type: 'app', token});

function getPriorSize() {
    const pr = process.env['CIRCLE_PULL_REQUEST'];
    if (!pr) {
        console.log('No pull request available.');
        return Promise.resolve(null);
    }

    const number = +pr.match(/\/(\d+)\/?$/)[1];
    console.log(`Pull request ${number}.`);

    return github.pullRequests.get({
        owner: 'mapbox',
        repo: 'mapbox-gl-native',
        number
    }).then(({data}) => {
        const head = process.env['CIRCLE_SHA1'];
        const base = data.base.sha;
        console.log(`Pull request target is ${base}.`);

        const mergeBase = execSync(`git merge-base ${base} ${head}`).toString().trim();
        console.log(`Merge base is ${mergeBase}.`);

        return github.checks.listForRef({
            owner: 'mapbox',
            repo: 'mapbox-gl-native',
            ref: mergeBase
        });
    }).then(({data}) => {
        const run = data.check_runs.find(run => run.name === name);
        if (!run) {
            console.log('No matching check found.');
            return Promise.resolve(null);
        }
        const prior = +run.output.summary.match(/`.*` is (\d+) bytes/)[1];
        console.log(`Prior size was ${prettyBytes(prior)}.`);
        return prior;
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
