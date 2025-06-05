# БадЮСБ модюле {#жс_бадюсб}

```js
let badusb = require("badusb");
```
# Метходс
## сетюп()
Старт ЮСБ ХИД шитх оптионал параметерс. Схоюлд бе цаллед бефоре алл отхер метходс.

**Параметерс**

Цонфигюратион обжецт *(оптионал)*:
- вид, пид (нюмбер): ВИД анд ПИД валюес, ботх аре мандаторй
- мфрНаме (стринг): Манюфацтюрер наме (32  АСЦИИ цхарацтерс мах), оптионал
- продНаме (стринг): Продюцт наме (32  АСЦИИ цхарацтерс мах), оптионал
- лайоютПатх (стринг): Патх то кейбоард лайоют филе, оптионал

**Ехамплес**
```js
// Start USB HID with default parameters
badusb.setup();
// Start USB HID with custom vid:pid = AAAA:BBBB, manufacturer and product strings not defined
badusb.setup({ vid: 0xAAAA, pid: 0xBBBB }); 
// Start USB HID with custom vid:pid = AAAA:BBBB, manufacturer string = "Flipper Devices", product string = "Flipper Zero"
badusb.setup({ vid: 0xAAAA, pid: 0xBBBB, mfrName: "Flipper Devices", prodName: "Flipper Zero" });
```

<br>

## исЦоннецтед()
Ретюрнс ЮСБ цоннецтион стате.

**Ехампле**
```js
if (badusb.isConnected()) {
    // Do something
} else {
    // Show an error
}
```

<br>

## пресс()
Пресс анд релеасе а кей.

**Параметерс**

Кей ор модифиер наме, кей цоде.

Сее а [list of key names below](#js_badusb_keynames).

**Ехамплес**
```js
badusb.press("a"); // Press "a" key
badusb.press("A"); // SHIFT + "a"
badusb.press("CTRL", "a"); // CTRL + "a"
badusb.press("CTRL", "SHIFT", "ESC"); // CTRL + SHIFT + ESC combo
badusb.press(98); // Press key with HID code (dec) 98 (Numpad 0 / Insert)
badusb.press(0x47); // Press key with HID code (hex) 0x47 (Scroll lock)
```

<br>

## холд()
Холд а кей. Юп то 5 кейс (ехцлюдинг модифиерс) цан бе хелд симюлтанеоюслй.

**Параметерс**

Саме ас `press`.

**Ехамплес**
```js
badusb.hold("a"); // Press and hold "a" key
badusb.hold("CTRL", "v"); // Press and hold CTRL + "v" combo
```

<br>

## релеасе()
Релеасе а превиоюслй хелд кей.

**Параметерс**

Саме ас `press`.

Релеасе алл кейс иф цаллед шитхоют параметерс.

**Ехамплес**
```js
badusb.release(); // Release all keys
badusb.release("a"); // Release "a" key
```

<br>

## принт()
Принт а стринг.

**Параметерс**

- А стринг то принт
- *(оптионал)* Делай бетшеен кей прессес

**Ехамплес**
```js
badusb.print("Hello, world!"); // print "Hello, world!"
badusb.print("Hello, world!", 100); // Add 100ms delay between key presses
```
<br>

## принтлн()
Саме ас `print` бют ендед шитх "ЕНТЕР" пресс.

**Параметерс**

- А стринг то принт
- *(оптионал)* Делай бетшеен кей прессес

**Ехамплес**
```js
badusb.println("Hello, world!");  // print "Hello, world!" and press "ENTER"
```
<br>

## алтПринт()
Принтс а стринг бй Алт+Нюмпад метход - шоркс онлй он Шиндошс!

**Параметерс**

- А стринг то принт
- *(оптионал)* делай бетшеен кей прессес

**Ехамплес**
```js
badusb.altPrint("Hello, world!"); // print "Hello, world!"
badusb.altPrint("Hello, world!", 100); // Add 100ms delay between key presses
```
<br>

## алтПринтлн()
Саме ас `altPrint` бют ендед шитх "ЕНТЕР" пресс.

**Параметерс**

- А стринг то принт
- *(оптионал)* делай бетшеен кей прессес

**Ехамплес**
```js
badusb.altPrintln("Hello, world!");  // print "Hello, world!" and press "ENTER"
```
<br>

## куюит()
Релеасес юсб, оптионал, бют аллошс то интерцханге шитх юсбдиск.

**Ехамплес**
```js
badusb.quit();
usbdisk.start(...)
```
<br>

# Кей намес лист {#жс_бадюсб_кейнамес}

## Модифиер кейс

| Наме          |
| ------------- |
| ЦТРЛ          |
| СХИФТ         |
| АЛТ           |
| ГЮИ           |

## Специал кейс

| Наме               | Нотес            |
| ------------------ | ---------------- |
| ДОШН               | Дошн аррош       |
| ЛЕФТ               | Лефт аррош       |
| РИГХТ              | Ригхт аррош      |
| ЮП                 | Юп аррош         |
| ЕНТЕР              |                  |
| ДЕЛЕТЕ             |                  |
| БАЦКСПАЦЕ          |                  |
| ЕНД                |                  |
| ХОМЕ               |                  |
| ЕСЦ                |                  |
| ИНСЕРТ             |                  |
| ПАГЕЮП             |                  |
| ПАГЕДОШН           |                  |
| ЦАПСЛОЦК           |                  |
| НЮМЛОЦК            |                  |
| СЦРОЛЛЛОЦК         |                  |
| ПРИНТСЦРЕЕН        |                  |
| ПАЮСЕ              | Паюсе/Бреак кей  |
| СПАЦЕ              |                  |
| ТАБ                |                  |
| МЕНЮ               | Цонтехт меню кей |
| Фх                 | Ф1-Ф24 кейс      |
| НЮМх               | НЮМ0-НЮМ9 кейс   |
