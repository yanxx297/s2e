#!/usr/bin/env python3

"""
Copyright (c) 2020, Vitaly Chipounov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import argparse
import datetime
import jinja2
import os

import pygit2


def render_file(destination, template, context):
    with open(template) as fp:
        template = jinja2.Template(fp.read())

    output = template.render(**context)

    with open(destination, 'w') as f:
        f.write(output)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--plugin-name', help='Plugin name', required=True)
    parser.add_argument('-o', '--output-dir', help='S2E source directory where to store the plugin', required=True)

    args = parser.parse_args()

    source_dir = os.path.dirname(os.path.abspath(__file__))
    cpp_template = os.path.join(source_dir, 'plugin.cpp.template')
    header_template = os.path.join(source_dir, 'plugin.h.template')

    cpp_plugin = os.path.join(args.output_dir, f'{args.plugin_name}.cpp')
    header_plugin = os.path.join(args.output_dir, f'{args.plugin_name}.h')

    cfg = pygit2.Config.get_global_config()
    for v in cfg.get_multivar('user.name'):
        name = v

    context = {
        'author': {
            'name': name,
            'year': datetime.datetime.now().year
        },
        'plugin': {
            'name': args.plugin_name,
            'description': 'Describe what the plugin does here'
        }
    }

    render_file(cpp_plugin, cpp_template, context)
    render_file(header_plugin, header_template, context)


if __name__ == '__main__':
    main()

