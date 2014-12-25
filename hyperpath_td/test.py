import sys
sys.path.append('/Users/tonny/Library/Developer/Xcode/DerivedData/hyperpath_td-bkixgxzlhvigykdqzonxrmyocpry/Build/Products/Release')
import mygraph
drmhelper = mygraph.Drmhelper("/Users/tonny/Documents/workspace/MyGraph_Fast/data/large.h5", "speeds")
g = drmhelper.make_graph("noexpress",190141, 453430)
