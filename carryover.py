#!/usr/bin/env python

import sqlite3
import sys
import zlib

in_db = sqlite3.connect('cache.db')

with in_db:
    in_db.row_factory = sqlite3.Row
    in_cur = in_db.cursor()
    in_cur.execute('select url, data, kind, compressed from http_cache where status = 1 order by kind asc')
    in_rows = in_cur.fetchall()
    out_db = sqlite3.connect('cache.mbtiles')
    with out_db:
        out_cur = out_db.cursor()
        out_cur.executescript("""
            drop table if exists tiles;
            create table tiles (zoom_level integer, tile_column integer, tile_row integer, tile_data blob);
            create unique index tile_index on tiles (zoom_level, tile_column, tile_row);
            drop table if exists metadata;
            create table metadata (name text,value text);
            create unique index name on metadata (name);
            """)
        for in_row in in_rows:
            kind = in_row['kind']
            data = in_row['data']
            if in_row['compressed'] == 1:
                data = zlib.decompress(data)
            if kind == 3:
                parts = in_row['url'].split('/')
                z = int(parts[4])
                x = int(parts[5])
                y = int(parts[6].split('.')[0])
                y = pow(2, z) - y - 1
                out_cur.execute('insert into tiles (zoom_level, tile_column, tile_row, tile_data) ' \
                                'values (?, ?, ?, ?)', (z, x, y, sqlite3.Binary(data)))
                print 'inserted tile %s,%s,%s (%s bytes)' % (z, x, y, len(data))
            elif kind in (2, 4, 5):
                if kind == 2:
                    prefix = 'gl_source'
                elif kind == 4:
                    prefix = 'gl_glyph'
                else:
                    prefix = 'gl_sprite_image'
                out_cur.execute('insert into metadata (name, value) values (?, ?)', 
                    (prefix + '_' + in_row['url'], sqlite3.Binary(data)))
                label = prefix.replace('gl_', '').replace('_', ' ')
                print 'inserted %s (%s bytes)' % (label, len(data))
            elif kind in (1, 6):
                if kind == 1:
                    prefix = 'gl_style'
                else:
                    prefix = 'gl_sprite_metadata'
                out_cur.execute('insert into metadata (name, value) values (?, ?)', 
                    (prefix + '_' + in_row['url'], data))
                label = prefix.replace('gl_', '').replace('_', ' ')
                print 'inserted %s (%s bytes)' % (label, len(data))
        out_db.commit()
