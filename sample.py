from chartrie import CharTrie, FrozenCharTrie
t = CharTrie()
print len(t)
t['hello'] = 49
print len(t)
t['hello1'] = 50
print len(t)
t['hello2'] = 51
print len(t)
t['hello3'] = 52
print len(t)
t['hello4'] = 53
print len(t)
t['help'] = 54
print len(t)
t['abc'] = 55
print len(t)
t['v'] = 56
print len(t)
t['a'] = 57
print len(t)
t[''] = 58
print len(t)
print 'get:', t['']
print 'get:', t['hel']
t.debug_print()
stream = t.dumps()
f = open('dump.trie', 'wb')
f.write(stream)
f.close()
d = FrozenCharTrie()
d.loads(stream)
print 'len(d)=',len(d)
print d.find_splits(d, 'hello1olleh')
print d.find_splits(d, 'a')