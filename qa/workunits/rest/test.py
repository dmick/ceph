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

def expect(url, method, respcode, contenttype, extra_hdrs=None):

    fdict = {'get':requests.get, 'put':requests.put}
    f = fdict[method.lower()]

    r = f(BASEURL + '/' + url, headers=extra_hdrs)
    if r.status_code != respcode:
        raise MyException('expected {0}, got {1}'.\
            format(respcode, r.status_code))
    r_contenttype = r.headers['content-type']
    if r_contenttype != 'application/' + contenttype:
        raise MyException('expected {0}, got {1}'.\
            format(r.headers['content-type'], contenttype))

    if r_contenttype == 'application/json':
        if r.json is None:
            raise MyException('Invalid JSON returned')

    if r_contenttype == 'application/xml':
        try:
            tree = xml.etree.ElementTree.fromstring(r.content)
        except Exception as e:
            raise MyException('Invalid XML returned: {0}'.format(str(e)))

    return r

if __name__ == '__main__':
    expect('auth/export', 'GET', 200, 'json')
    expect('auth/export.json', 'GET', 200, 'json')
    expect('auth/export.xml', 'GET', 200, 'xml')
    expect('auth/export.xml', 'GET', 200, 'xml', {'accept':'application/xml'})
    print 'OK'
