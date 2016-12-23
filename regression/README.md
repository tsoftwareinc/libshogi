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
2.56user 0.06system 0:02.63elapsed 99%CPU (0avgtext+0avgdata 12776maxresident)k
0inputs+0outputs (0major+850minor)pagefaults 0swaps
OK
TEST MOVE   :
72.47user 0.15system 1:12.64elapsed 99%CPU (0avgtext+0avgdata 12836maxresident)k
0inputs+0outputs (0major+861minor)pagefaults 0swaps
OK
TEST EFFECT :
6.48user 0.06system 0:06.55elapsed 99%CPU (0avgtext+0avgdata 13132maxresident)k
0inputs+0outputs (0major+415minor)pagefaults 0swaps
OK
TEST PIN    :
3.51user 0.10system 0:03.61elapsed 99%CPU (0avgtext+0avgdata 13012maxresident)k
0inputs+0outputs (0major+396minor)pagefaults 0swaps
OK
TEST FAST   :
66.75user 0.09system 1:06.85elapsed 99%CPU (0avgtext+0avgdata 12832maxresident)k
0inputs+0outputs (0major+380minor)pagefaults 0swaps
OK
TEST CAPT   :
18.80user 0.10system 0:18.91elapsed 99%CPU (0avgtext+0avgdata 12960maxresident)k
0inputs+0outputs (0major+406minor)pagefaults 0swaps
OK
TEST OUTE   :
74.50user 0.09system 1:14.60elapsed 99%CPU (0avgtext+0avgdata 13052maxresident)k
0inputs+0outputs (0major+419minor)pagefaults 0swaps
OK
TEST OFST   :
77.43user 0.14system 1:17.59elapsed 99%CPU (0avgtext+0avgdata 12976maxresident)k
0inputs+0outputs (0major+871minor)pagefaults 0swaps
OK
TEST UNDO   :
120.31user 0.11system 2:00.44elapsed 99%CPU (0avgtext+0avgdata 12904maxresident)k
0inputs+0outputs (0major+380minor)pagefaults 0swaps
OK
TEST GETOUT :
3.18user 0.04system 0:03.23elapsed 99%CPU (0avgtext+0avgdata 12716maxresident)k
0inputs+0outputs (0major+846minor)pagefaults 0swaps
OK
TEST VALUE  :
3.28user 0.04system 0:03.32elapsed 99%CPU (0avgtext+0avgdata 12556maxresident)k
0inputs+0outputs (0major+841minor)pagefaults 0swaps
OK
TEST HASH   :
10.72user 0.76system 0:11.48elapsed 99%CPU (0avgtext+0avgdata 3769980maxresident)k
0inputs+0outputs (0major+939646minor)pagefaults 0swaps
OK
```

