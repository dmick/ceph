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

def expect(url, method, respcode, contenttype, extra_hdrs=None, data=None):

    fdict = {'get':requests.get, 'put':requests.put}
    f = fdict[method.lower()]
    r = f(BASEURL + '/' + url, headers=extra_hdrs, data=data)

    if r.status_code != respcode:
        fail(r, 'expected {0}, got "{1}"'.format(respcode, r.status_code))
    r_contenttype = r.headers['content-type']

    if contenttype in ['json', 'xml']:
        contenttype = 'application/' + contenttype
    elif contenttype:
        contenttype = 'text/' + contenttype

    if contenttype and r_contenttype != contenttype:
        fail(r,  'expected {0}, got "{1}"'.format(contenttype, r_contenttype))

    if contenttype.startswith('application'):
        if r_contenttype == 'application/json':
            # may raise
            try:
                assert(r.json != None)
            except Exception as e:
                fail(r, 'Invalid JSON returned: "{0}"'.format(str(e)))

        if r_contenttype == 'application/xml':
            try:
                # if it's there, squirrel it away for use in the caller
                r.tree = xml.etree.ElementTree.fromstring(r.content)
            except Exception as e:
                fail(r, 'Invalid XML returned: "{0}"'.format(str(e)))

    return r

JSONHDR={'accept':'application/json'}
XMLHDR={'accept':'application/xml'}

if __name__ == '__main__':
    '''
    expect('auth/export', 'GET', 200, 'plain')
    expect('auth/export.json', 'GET', 200, 'json')
    expect('auth/export.xml', 'GET', 200, 'xml')
    expect('auth/export', 'GET', 200, 'json', JSONHDR)
    expect('auth/export', 'GET', 200, 'xml', XMLHDR)

    expect('auth/add?entity=client.xx&'
           'caps=mon&caps=allow&caps=osd&caps=allow *', 'PUT', 200, 'json',
            JSONHDR)

    r = expect('auth/export?entity=client.xx', 'GET', 200, 'plain')
    # must use text/plain; default is application/x-www-form-urlencoded
    expect('auth/add.json?entity=client.xx', 'PUT', 200, 'json',
           {'Content-Type':'text/plain'}, data=r.content)

    r = expect('auth/list', 'GET', 200, 'plain')
    assert('client.xx' in r.content)

    r = expect('auth/list.json', 'GET', 200, 'json')
    dictlist = r.json['output']['auth_dump']
    xxdict = [d for d in dictlist if d['entity'] == 'client.xx'][0]
    assert(xxdict)
    assert('caps' in xxdict)
    assert('mon' in xxdict['caps'])
    assert('osd' in xxdict['caps'])

    expect('auth/get-key?entity=client.xx', 'GET', 200, 'json', JSONHDR)
    expect('auth/print-key?entity=client.xx', 'GET', 200, 'json', JSONHDR)
    expect('auth/print_key?entity=client.xx', 'GET', 200, 'json', JSONHDR)

    expect('auth/caps?entity=client.xx&caps=osd&caps=allow rw', 'PUT', 200,
           'json', JSONHDR)
    r = expect('auth/list.json', 'GET', 200, 'json')
    dictlist = r.json['output']['auth_dump']
    xxdict = [d for d in dictlist if d['entity'] == 'client.xx'][0]
    assert(xxdict)
    assert('caps' in xxdict)
    assert(not 'mon' in xxdict['caps'])
    assert('osd' in xxdict['caps'])
    assert(xxdict['caps']['osd'] == 'allow rw')

    # export/import/export, compare
    r = expect('auth/export', 'GET', 200, 'plain')
    exp1 = r.content
    assert('client.xx' in exp1)
    r = expect('auth/import', 'PUT', 200, 'plain',
               {'Content-Type':'text/plain'}, data=r.content)
    r2 = expect('auth/export', 'GET', 200, 'plain')
    assert(exp1 == r2.content)
    expect('auth/del?entity=client.xx', 'PUT', 200, 'json', JSONHDR)

    '''
    # df
    assert('GLOBAL' in expect('df', 'GET', 200, 'plain').content)
    assert('CATEGORY' in expect('df?detail=detail', 'GET', 200, 'plain').content)
    # test param with no value (treated as param=param)
    assert('CATEGORY' in expect('df?detail', 'GET', 200, 'plain').content)

    r = expect('df', 'GET', 200, 'json', JSONHDR)
    assert('total_used' in r.json['output']['stats'])
    r = expect('df?detail', 'GET', 200, 'json', JSONHDR)
    assert('rd_kb' in r.json['output']['pools'][0]['stats'])

    r = expect('df', 'GET', 200, 'xml', XMLHDR)
    assert(r.tree.find('output/stats/stats/total_used') is not None)
    r = expect('df?detail', 'GET', 200, 'xml', XMLHDR)
    assert(r.tree.find('output/stats/pools/pool/stats/rd_kb') is not None)

    expect('fsid', 'GET', 200, 'json', JSONHDR)
    expect('health', 'GET', 200, 'json', JSONHDR)
    # XXX health detail is broken; adds detail without regard for format
    # expect('health?detail', 'GET', 200, 'json', JSONHDR)
    expect('health?detail', 'GET', 200, 'plain')

    # XXX no ceph -w equivalent yet

    # XXX random content type is a bit stinky
    expect('mds/cluster_down', 'PUT', 200, '')
    # failure if down
    expect('mds/cluster_down', 'PUT', 400, '')
    expect('mds/cluster_up', 'PUT', 200, '')
    # failure if up
    expect('mds/cluster_up', 'PUT', 400, '')

    print 'OK'
