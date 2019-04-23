import java from './generators/java';

export function generators(type) {
    let fs = require('fs');
  
    console.log('Generating tests for '+ type + ':');
    var api = { 
        'operations'      : JSON.parse(fs.readFileSync('config/supported-operations.json', 'utf8')),
        'properties'      : JSON.parse(fs.readFileSync('config/supported-properties.json', 'utf8')),
        'java-ignores'    : JSON.parse(fs.readFileSync('config/ignores.json', 'utf8')),
        'node-ignores'    : JSON.parse(fs.readFileSync('../../node/test/ignores.json', 'utf8'))
    };
    if (type === 'android') {
        java(api, type);
    }
    console.log('');
}
