#pylint: disable=missing-docstring
import os
import collections
import logging

import mooseutils

LOG = logging.getLogger(__name__)

class Requirement(object):
    """struct for storing Requirement information."""
    def __init__(self, name=None, path=None, filename=None, text=None, design=None, issues=None):
        self.name = name
        self.path = path
        self.filename = filename
        self.text = text
        self.design = design
        self.issues = issues
        self.label = None # added by get_requirements function

    def __str__(self):
        frmt = '{}:\n    Text: {}\n    Design: {}\n    Issues: {}'
        return frmt.format(self.name, self.text, repr(self.design), repr(self.issues))

def get_requirements(directories, specs):
    """
    Build requirements dictionary from the provided directories.
    """
    out = collections.defaultdict(list)
    for location in directories:
        for filename in mooseutils.git_ls_files(location):
            if not os.path.isfile(filename):
                continue
            fname = os.path.basename(filename)
            if fname in specs:
                _add_requirements(out, location, filename)

    for i, requirements in enumerate(out.itervalues()):
        for j, req in enumerate(requirements):
            req.label = "F{}.{}".format(i+1, j+1)
    return out

def _add_requirements(out, location, filename):
    """Opens tests specification and extracts requirement items."""
    root = mooseutils.hit_load(filename)
    design = root.children[0].get('design', None)
    issues = root.children[0].get('issues', None)
    for child in root.children[0]:
        if 'requirement' in child:

            local_design = child.get('design', design)
            if local_design is None:
                msg = "The 'design' parameter is missing from '%s' in %s. It must be defined at " \
                      "the top level and/or within the individual test specification. It " \
                      "should contain a space separated list of filenames."
                LOG.error(msg, child.name, filename)
                local_design = ''

            local_issues = child.get('issues', issues)
            if local_issues is None:
                msg = "The 'issues' parameter is missing from '%s' in %s. It must be defined at " \
                      "the top level and/or within the individual test specification. It " \
                      "should contain a space separated list of issue numbers (include the #)."
                LOG.error(msg, child.name, filename)
                local_issues = ''

            req = Requirement(name=child.name,
                              path=os.path.relpath(os.path.dirname(filename), location),
                              filename=filename,
                              text=unicode(child['requirement']),
                              design=local_design.split(),
                              issues=local_issues.split())
            group = os.path.relpath(filename, location).split('/')[0]
            out[group].append(req)
