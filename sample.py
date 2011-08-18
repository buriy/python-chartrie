from chartrie import CharTrie
t = CharTrie()
print len(t)
t['hello'] = 1
print len(t)
t['hello1'] = 2
print len(t)
t['hello2'] = 3
print len(t)
t['hello3'] = 4
print len(t)
t['hello4'] = 5
print len(t)
t['help'] = 6
print len(t)
t['abc'] = 7
print len(t)
t['v'] = 8
print len(t)
t['a'] = 9
print len(t)
t.debug_print()
print t.serialize()
