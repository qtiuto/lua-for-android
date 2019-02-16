
using 'java.lang'
using 'java.util'

--type_ test
assert(type_("Object")==Object)
assert(type_("ArrayList")==ArrayList)

--import test
assert(Object==import("java.lang.Object"))
assert(ArrayList==import("java.util.ArrayList"))

--new test
assert(new )