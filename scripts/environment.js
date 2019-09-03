#!/usr/bin/env node

// Output some `export` commands with a few extra environment variables. They'll be evaluated
// into the build environment and available for use in later steps.

const github = require('@octokit/rest')();
const {execSync} = require('child_process');

const pr = process.env['CIRCLE_PULL_REQUEST'];
if (pr) {
    const number = +pr.match(/\/(\d+)\/?$/)[1];
    return github.pullRequests.get({
        owner: 'mapbox',
        repo: 'mapbox-gl-native',
        number
    }).then(({data}) => {
        const base = data.base.ref;
        const head = process.env['CIRCLE_SHA1'];
        const mergeBase = execSync(`git merge-base origin/${base} ${head}`).toString().trim();

        console.log(`export CIRCLE_TARGET_BRANCH=${base}`);
        console.log(`export CIRCLE_MERGE_BASE=${mergeBase}`);
    });
} else {
    const head = process.env['CIRCLE_SHA1'];
    for (const sha of execSync(`git rev-list --max-count=500 ${head}`).toString().trim().split('\n')) {
        const base = execSync(`git branch -r --contains ${sha} origin/master origin/release-*`).toString().split('\n')[0].trim().replace(/^origin\//, '');
        if (base) {
            const mergeBase = execSync(`git merge-base origin/${base} ${head}`).toString().trim();
            console.log(`export CIRCLE_TARGET_BRANCH=${base}`);
            console.log(`export CIRCLE_MERGE_BASE=${mergeBase}`);
            break;
        }
    }
}
