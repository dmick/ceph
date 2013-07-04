#!/usr/bin/python

import exceptions
import os
# import nosetests
import requests
import sys
import xml.etree.ElementTree

BASEURL = os.environ.get('BASEURL', 'http://localhost:5000/api/v0.1')

class MyException(Exception):
    pass

def fail(r, msg):
    print >> sys.stderr, 'FAILURE: url ', r.url
    print >> sys.stderr, msg
    print >> sys.stderr, 'Response content: ', r.content
    print >> sys.stderr, 'Headers: ', r.headers
    sys.exit(1)

def expect(url, method, respcode, contenttype='json', extra_hdrs=None):

    fdict = {'get':requests.get, 'put':requests.put}
    f = fdict[method.lower()]

    r = f(BASEURL + '/' + url, headers=extra_hdrs)
    if r.status_code != respcode:
        fail(r, 'expected {0}, got "{1}"'.format(respcode, r.status_code))
    r_contenttype = r.headers['content-type']

    # allow null contenttype to avoid checking
    if contenttype:
        if r_contenttype != 'application/' + contenttype:
            fail(r,  'expected {0}, got "{1}"'.\
                format(contenttype, r_contenttype))

        if r_contenttype == 'application/json':
            # may raise
            try:
                assert(r.json != None)
            except Exception as e:
                fail(r, 'Invalid JSON returned: "{0}"'.format(str(e)))

        if r_contenttype == 'application/xml':
            try:
                tree = xml.etree.ElementTree.fromstring(r.content)
            except Exception as e:
                fail(r, 'Invalid XML returned: "{0}"'.format(str(e)))

    return r

if __name__ == '__main__':
    expect('auth/export', 'GET', 200)
    expect('auth/export.json', 'GET', 200)
    expect('auth/export.xml', 'GET', 200, 'xml')
    expect('auth/export.xml', 'GET', 200, 'xml', {'accept':'application/xml'})

    expect('auth/add?entity=client.xx&caps=mon&caps=allow&caps=osd&caps=allow *', 'PUT', 200, 'json')
    # r = expect('auth/list', 'GET', 200)
    # assert('client.xx' in r['???']['???'])
    r = expect('auth/list', 'GET', 200, 'plain')
    assert('client.xx' in r.content)
    print 'OK'
