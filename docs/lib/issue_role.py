# -*- coding: utf-8 -*-
"""
    issue role
    ~~~~~~~~~~

    An interpreted text role to link docs to issues.

    :copyright: Copyright 2013-2020 by Daniele Varrazzo.
"""

import re
from docutils import nodes, utils
from docutils.parsers.rst import roles


def issue_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    cfg = inliner.document.settings.env.app.config
    if cfg.issue_url is None:
        msg = inliner.reporter.warning(
            "issue not configured: please configure issue_url in conf.py"
        )
        prb = inliner.problematic(rawtext, rawtext, msg)
        return [prb], [msg]

    rv = [nodes.Text(name + ' ')]
    tokens = re.findall(r'(#?\d+)|([^\d#]+)', text)
    for issue, noise in tokens:
        if issue:
            num = int(issue.replace('#', ''))
            url = cfg.issue_url % num
            roles.set_classes(options)
            node = nodes.reference(
                issue, utils.unescape(issue), refuri=url, **options
            )

            rv.append(node)

        else:
            assert noise
            rv.append(nodes.Text(noise))

    return rv, []


def setup(app):
    app.add_config_value('issue_url', None, 'env')
    app.add_role('issue', issue_role)
    app.add_role('issues', issue_role)
