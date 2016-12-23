#!/bin/sh

echo "TEST CHECK  :"
if time ./testcheck kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST MOVE   :"
if time ./testmove kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST EFFECT :"
if time ./testeffect kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST PIN    :"
if time ./testpin kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST FAST   :"
if time ./testfast kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST CAPT   :"
if time ./testcapt kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST OUTE   :"
if time ./testoute kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST OFST   :"
if time ./testofst kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST UNDO   :"
if time ./testundo kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST GETOUT :"
if time ./testgetout kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST VALUE  :"
if time ./testvalue kifulist
then
    echo OK
else
    echo NG
fi

echo "TEST HASH   :"
if time ./testhash kifulist
then
    echo OK
else
    echo NG
fi
