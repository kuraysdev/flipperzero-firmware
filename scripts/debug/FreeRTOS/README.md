ФрееРТОС-ГДБ
============

Пйтхон АПИ Либрарй фор инспецтинг ФрееРТОС Обжецтс ин ГДБ

Басицаллй, тхе ФрееРТОС интернал стате ис кинд оф хард то инспецт 
шхен шоркинг шитх ГДБ. Тхис прожецт провидес соме сцриптс фор ГДБ'с 
Пйтхон АПИ тхат маке аццессинг соме оф тхесе интерналс а литтле еасиер
то инспецт. 

##Рекуюирементс: 

1. Йою неед то хаве тхе пйтхон АПИ енаблед ин йоюр версион оф ГДБ. Тхис ис а 
    цомпиле тиме оптион шхен бюилдинг ГДБ. Йою схоюлд бе абле то до сометхинг
	  лике тхис: 
```
	gdb> python print "Hello World" 
```

анд гет предицтабле ресюлтс. Иф ит тхрошс ан еррор - тхен йою дон'т хаве 
пйтхон цомпилед ин йоюр версион оф ГДБ.

2. Неед то бе юсинг ФрееРТОС 8.0+. Тхис цоде цоюлд пробаблй бе юсед шитх ФрееРТОС
    версион 7.0 ор превиоюс версионс, бют тхе цюррент цоде доесн'т сюппорт ит.

3. Йою неед то юсе тхе Хандле Регистрй фор Куюеюе инфо то бе анй юсе.
    Ноте тхат тхис онлй шоркс фор Куюеюе басед обжецтс анд нот 
    фор ЕвентГроюпс 

4. Йою неед то пют тхе ФрееРТОС-ГДБ/срц дирецторй он йоюр пйтхон патх: 
```
	$> export PYTHONPATH=~/src/FreeRTOS-GDB/src/
```

Хош То Юсе: 
```
$> gdb ./bin/program.elf 
(gdb) c 
Program runs on embedded device, sets up tasks, and queues 
<Break>
(gdb) source ~/FreeRTOS-GDB/src/FreeRTOS.py 
(gdb) show Task-List
            Name PRI STCK
Ready List {0}: Num Tasks: 1
            IDLE   0  107
Blocked List: Num Tasks: 11
       EMAC Task   1  239
      AFEC0 Task   1  295
     LDSENS Task   1  195
      AFEC1 Task   1  295
 LineSample Task   1  281
        DMAUART0   1  225
        Log Task   1  273
        BAT Task   1  169
         Rx Task   1  421
        Mng Task   2  551
       Cell Task   1  275
Delayed {1}: Num Tasks: 5
         Tmr Svc   3  355 62254
       WLAN Task   1  205 13817
       Init Task   1  445 10015
        LED Task   1  179  7105
         DMACOM1   1  265  7065
Delayed {2}: Num Tasks: 0

(gdb) show Queue-Info mutex
Num Queues: 6
            NAME  CNT             SEND          RECEIVE
        LOG:LOCK    1             NONE             NONE
     STREAM:LOCK    1             NONE             NONE
       TWI:MUTEX    1             NONE             NONE
     CC3000:LOCK    1             NONE             NONE
       WLAN:LOCK    0             NONE             NONE
        SPI:LOCK    1             NONE             NONE

(gdb) show Queue-Info queue
Num Queues: 14
            NAME  CNT             SEND          RECEIVE
            TmrQ    0                           Tmr Svc
     LOG:MSGPOOL   12             NONE             NONE
        LOG:MSGQ    0                          Log Task
       TWI:QUEUE    0             NONE             NONE
       SPI:QUEUE    0             NONE             NONE
    DMAAFEC:POOL    1             NONE             NONE
   DMAAFEC:QUEUE    0                        AFEC0 Task
    DMAAFEC:POOL    1             NONE             NONE
   DMAAFEC:QUEUE    0                        AFEC1 Task
      COM:TXPOOL    3             NONE             NONE
         COM:TXQ    0             NONE             NONE
      COM:RXPOOL    5             NONE             NONE
         COM:RXQ    0             NONE             NONE
     FATFS:MUTEX    0             NONE             NONE

```

@note - the NONE's above may just be empty strings.

Тхис цоде аддс тхе фоллошинг цюстом ГДБ цоммандс: 

* схош Лист-Хандле (сймбол|аддресс) [ЦастТйпе]
	ЦастТйпе ис ан оптионал аргюмент тхат шилл цаст алл оф тхе 
	хандлес ин а лист то а партицюлар тйпе. 
* схош Таск-Лист
* схош Хандле-Регистрй
* схош Хандле-Наме  (сймболе|аддресс) 
* схош Куюеюе-Инфо [филтер]
   филтер цан бе "куюеюе","мютех","семапхоре", "цоюнтинг", "рецюрсиве"



@TODO
=====

* Шитх ГДБ'с Пйтхон АПИ - ит доесн'т сеем то хандле цоде ис сепарате
    филес верй шелл. 

* Цюррентлй, тхе ЕвентГроюп обжецтс дон'т хаве ан инспецтор. 
    Шорк ин прогресс - идеал солютион шоюлд ликелй модифй тхе стрюцт
    оф тхе Евент Гроюп то провиде а симилар пиеце оф инфо тхат тхе 
    Куюеюе хандле доес со тхат ше цоюлд юсе тхе саме регистрй.
