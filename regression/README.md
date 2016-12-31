# libshogi/regression

## REQUIREMENTS

   The programs in this directory are for doing regression test.
   'lesserpyon' is required to crosscheck. You also need a large
   amount of kifu files in CSA format.

   - kifulist

   The file contains a list of file pathes.

   - Memory

   testhash uses a lot of memory. More than 10 GB is required for
   35,000 kifu files. You may comment out testhash in test.sh
   script.

   - Installation of lesserpyon

   1) extract lesserpyon3.zip into lesserpyon directory.

   2) change encode to UTF-8 and modify line-end to unix style.
      (ex.
       for i in *
       do
         nkf -Lu -w --overwrite $i
       done

   3) patch below

```
   diff -u -Bbw lesserpyon.original/kyokumen.h lesserpyon/kyokumen.h
   --- lesserpyon.original/kyokumen.h	2004-12-24 22:28:00.000000000 +0900
   +++ lesserpyon/kyokumen.h	2016-12-01 21:17:47.606515859 +0900
   @@ -337,7 +337,7 @@
 	   static uint64 HI_BIT_TBL[3];
 	   static TsumeVal *FindFirst(uint64 KyokumenHashVal);
 	   static TsumeVal *FindNext(TsumeVal* Now);
   -	static uint64 TsumeHash::CalcHand(int Motigoma[]);
   +	static uint64 CalcHand(int Motigoma[]);
    public:
 	   static void Clear();
 	   static void Add(uint64 KyokumenHashVal,uint64 HandHashVal,int Motigoma[],int mate,Te te);
```


## BENCHMARK

   Tested on Intel Core i3-6100 CPU @ 3.70GHz. Using 34983 kifu files containing
   about 100 positions each :

```
TEST CHECK  :
3.68user 1.01system 3:16.43elapsed 2%CPU (0avgtext+0avgdata 12720maxresident)k
300736inputs+0outputs (0major+845minor)pagefaults 0swaps
OK
TEST MOVE   :
73.60user 0.15system 1:13.76elapsed 99%CPU (0avgtext+0avgdata 13232maxresident)k
0inputs+0outputs (0major+421minor)pagefaults 0swaps
OK
TEST EFFECT :
6.72user 0.04system 0:06.77elapsed 99%CPU (0avgtext+0avgdata 12908maxresident)k
0inputs+0outputs (0major+876minor)pagefaults 0swaps
OK
TEST PIN    :
3.36user 0.07system 0:03.43elapsed 99%CPU (0avgtext+0avgdata 12836maxresident)k
0inputs+0outputs (0major+861minor)pagefaults 0swaps
OK
TEST FAST   :
69.04user 0.14system 1:09.23elapsed 99%CPU (0avgtext+0avgdata 12740maxresident)k
0inputs+0outputs (0major+863minor)pagefaults 0swaps
OK
TEST CAPT   :
19.14user 0.09system 0:19.24elapsed 99%CPU (0avgtext+0avgdata 12836maxresident)k
0inputs+0outputs (0major+862minor)pagefaults 0swaps
OK
TEST OUTE   :
76.92user 0.13system 1:17.06elapsed 99%CPU (0avgtext+0avgdata 12808maxresident)k
0inputs+0outputs (0major+864minor)pagefaults 0swaps
OK
TEST OFST   :
80.98user 0.16system 1:21.16elapsed 99%CPU (0avgtext+0avgdata 12728maxresident)k
0inputs+0outputs (0major+862minor)pagefaults 0swaps
OK
TEST UNDO   :
117.66user 0.16system 1:57.87elapsed 99%CPU (0avgtext+0avgdata 12916maxresident)k
0inputs+0outputs (0major+383minor)pagefaults 0swaps
OK
TEST GETOUT :
3.38user 0.07system 0:03.45elapsed 99%CPU (0avgtext+0avgdata 12924maxresident)k
0inputs+0outputs (0major+379minor)pagefaults 0swaps
OK
TEST VALUE  :
3.29user 0.06system 0:03.36elapsed 99%CPU (0avgtext+0avgdata 13256maxresident)k
0inputs+0outputs (0major+433minor)pagefaults 0swaps
OK
TEST HASH   :
11.18user 0.80system 0:12.01elapsed 99%CPU (0avgtext+0avgdata 3769936maxresident)k
0inputs+0outputs (0major+939646minor)pagefaults 0swaps
OK
```

