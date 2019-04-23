import yargs from 'yargs';
import { generators } from './generators';

function parser(language) {
    generators(language)
}

function builder(yargs) {
    return yargs.boolean('list');
}

yargs
    .command('[android]', 'generate Android render tests', builder, parser('android'))
    .parse();




