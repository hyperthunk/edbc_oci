
#include "cspec.h"

static int nbefore_each;

describe "CSpec before_each"
    before_each
        ++nbefore_each;
    end
  
    it "should be called before each spec"
        nbefore_each should equal 1
    end
  
    it "should be called before each spec"
        nbefore_each should equal 2
    end
end
