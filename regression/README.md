# libshogi/regression

1. REQUIREMENTS

   The programs in this directory are for doing regression test.
   'lesserpyon' is required for testing to crosscheck. You also
   need a large amount of kifu files in CSA format.

   - kifulist

   The file contains a list of file pathes.

   - Memory

   According to number of kifu files, testhash uses a lot of
   memory. More than 10 GB is required for 35,000 files. You
   may comment out testhash in test.sh script.

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


2. BENCHMARK

   Tested on Intel Core i3-6100 CPU @ 3.70GHz. Using 34983 kifu files containing
   about 100 positions each :

```
   TEST CHECK  :
   3.14user 0.08system 0:03.23elapsed 99%CPU (0avgtext+0avgdata 12708maxresident)k
   0inputs+0outputs (0major+844minor)pagefaults 0swaps
   OK
   TEST MOVE   :
   71.51user 0.11system 1:11.63elapsed 99%CPU (0avgtext+0avgdata 12988maxresident)k
   0inputs+0outputs (0major+399minor)pagefaults 0swaps
   OK
   TEST EFFECT :
   6.80user 0.08system 0:06.88elapsed 99%CPU (0avgtext+0avgdata 13016maxresident)k
   0inputs+0outputs (0major+880minor)pagefaults 0swaps
   OK
   TEST PIN    :
   3.40user 0.09system 0:03.49elapsed 99%CPU (0avgtext+0avgdata 13216maxresident)k
   0inputs+0outputs (0major+400minor)pagefaults 0swaps
   OK
   TEST FAST   :
   66.19user 0.11system 1:06.31elapsed 99%CPU (0avgtext+0avgdata 12684maxresident)k
   0inputs+0outputs (0major+864minor)pagefaults 0swaps
   OK
   TEST UNDO   :
   124.56user 0.12system 2:04.70elapsed 99%CPU (0avgtext+0avgdata 12752maxresident)k
   0inputs+0outputs (0major+378minor)pagefaults 0swaps
   OK
   TEST GETOUT :
   3.16user 0.04system 0:03.21elapsed 99%CPU (0avgtext+0avgdata 12756maxresident)k
   0inputs+0outputs (0major+390minor)pagefaults 0swaps
   OK
   TEST VALUE  :
   3.29user 0.07system 0:03.37elapsed 99%CPU (0avgtext+0avgdata 12784maxresident)k
   0inputs+0outputs (0major+846minor)pagefaults 0swaps
   OK
   TEST HASH   :
   9.80user 0.52system 0:10.34elapsed 99%CPU (0avgtext+0avgdata 2517704maxresident)k
   0inputs+0outputs (0major+626602minor)pagefaults 0swaps
   OK
```

