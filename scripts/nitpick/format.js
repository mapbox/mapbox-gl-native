#!/usr/bin/env node

const jwt = require('jsonwebtoken');
const github = require('@octokit/rest')();
const fs = require('fs');

const {execSync} = require('child_process');
const parseDiff = require('parse-diff');

const SIZE_CHECK_APP_ID = 14028;
const SIZE_CHECK_APP_INSTALLATION_ID = 229425;

process.on('unhandledRejection', error => {
    console.log(error);
    process.exit(1)
});

async function getAnnotations() {
    const env = await require('../environment')();
    if (!env.mergeBase) {
        console.log('No merge base available.');
        return;
    }

    let annotations = [];
    parseDiff(execSync(`git diff ${env.mergeBase} ${env.head} --unified=0 --dst-prefix='' *.cpp *.hpp`).toString()).forEach(file => {
        // Can't use git diff's exclude syntax just yet because Circle CI's git is too old.
        if (/^vendor\//.test(file.to)) {
            return;
        }

        const chunks = file.chunks.filter(chunk => chunk.newLines || !chunk.oldLines);
        if (!chunks.length) {
            return;
        }

        const lines = chunks.map(chunk => {
            const start = chunk.newStart;
            const end = chunk.newStart + (chunk.newLines ? chunk.newLines - 1 : 0);
            return `-lines=${start}:${end}`;
        });

        const formatted = execSync([ '${CLANG_FORMAT:-clang-format}', lines.join(' '), file.to].join(' ')).toString();

        // diff exits with code 1 if the files differ, and code 2 if something went wrong.
        parseDiff(execSync(`diff --unified=0 <(git show ${env.head}:${file.to}) - || [ $? -eq 1 ]`, { input: formatted, shell: '/bin/bash' }).toString()).forEach(diff => {
            diff.chunks.forEach(function (chunk) {
                const start = chunk.oldStart;
                const end = chunk.oldStart + (chunk.oldLines ? chunk.oldLines - 1 : 0);
                annotations.push({
                    path: file.to,
                    start_line: start,
                    end_line: end,
                    annotation_level: 'notice',
                    title: `consider adjusting the code formatting:`,
                    message: chunk.changes.map(change => change.content).join('\n')
                });
            });
        });
    });

    return annotations;
}

async function postToGithub() {
    const annotations = await getAnnotations();

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
            return github.checks.create({
                owner: 'mapbox',
                repo: 'mapbox-gl-native',
                name: 'Code Formatting',
                head_branch: process.env['CIRCLE_BRANCH'],
                head_sha: process.env['CIRCLE_SHA1'],
                status: 'completed',
                conclusion: 'neutral',
                completed_at: new Date().toISOString(),
                output: {
                    title: 'Code Formatting',
                    summary: 'Proposed code formatting changes',
                    annotations: annotations
                }
            });
        });

}

const colors = {
    file: (text) => `\x1b[1m\x1b[39m${text}\x1b[0m`,
    removal: (text) => `\x1b[31m${text}\x1b[0m`,
    addition: (text) => `\x1b[32m${text}\x1b[0m`
};

async function printToCommandLine() {
    const annotations = await getAnnotations();

    for (const annotation of annotations) {
        console.log(colors.file(`${annotation.path}:${annotation.start_line}`));
        for (const line of annotation.message.split('\n')) {
            if (line[0] === '-') {
                console.log(colors.removal(line.replace(/\s+$/, '\x1b[97;41m$&')));
            } else if (line[0] === '+') {
                console.log(colors.addition(line.replace(/\s+$/, '\x1b[97;42m$&')));
            } else {
                console.log(line);
            }
        }
        console.log('');
    }
}

const pk = process.env['SIZE_CHECK_APP_PRIVATE_KEY'];
if (pk) {
    postToGithub();
} else {
    printToCommandLine();
}
