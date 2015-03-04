#/bin/env python
# -*-coding:utf8-*-
'''
# Any issues or improvements please contact jacob-chen@iotwrt.com
'''
import urllib,sys,os

if __name__ == '__main__':
    try:
        target_dir = sys.argv[1]

        domain = 'https://hosts.huhamhire.com/update/'

        os.system('amh hosts stop');

        f = urllib.urlopen(domain + 'hostsinfo.json')
        get = f.read()

        with open(target_dir + "/hostsinfo.json", "wb") as code:     
            code.write(get)

        os.system('chgrp www ' + target_dir + "/hostsinfo.json");
        os.system('chown www ' + target_dir + "/hostsinfo.json");

        f = urllib.urlopen(domain + 'hostslist.data')

        get = f.read()

        with open(target_dir + "/hostslist.data", "wb") as code:     
            code.write(get)   

        os.system('chgrp www ' + target_dir + "/hostslist.data");
        os.system('chown www ' + target_dir + "/hostslist.data");

        os.system('amh hosts start');
    except:
        sys.exit(-1)
        
    sys.exit(0)
    


