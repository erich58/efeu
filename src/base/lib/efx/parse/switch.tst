#!/usr/bin/env .build/script.tst

int test_switch(int c)
{
	switch (c)
	{
	case 1: return 1;
	case 2: return 2;
	default: return 0;
	}
}


EXPECT(test_switch(1) == 1);
EXPECT(test_switch(2) == 2);
EXPECT(test_switch(3) == 0);
