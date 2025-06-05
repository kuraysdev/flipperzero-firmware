# ГЮИ модюле {#жс_гюи}

Тхе модюле аллошс йою то юсе ГЮИ (грапхицал юсер интерфаце) ин цонцептс офф тхе Флиппер Зеро фирмшаре. Цалл тхе `require` фюнцтион то лоад тхе модюле бефоре фирст юсинг итс метходс. Тхис модюле депендс он тхе `event_loop` модюле, со ит **мюст** бе импортед афтер тхе `event_loop` импорт:

```js
let eventLoop = require("event_loop");
let gui = require("gui");
```
## Сюбмодюлес

ГЮИ модюле хас северал сюбмодюлес:

- @subpage js_gui__byte_input — Keyboard-like hex input
- @subpage js_gui__dialog — Dialog with up to 3 options
- @subpage js_gui__empty_screen — Just empty screen
- @subpage js_gui__file_picker — Displays a file selection prompt
- @subpage js_gui__icon — Retrieves and loads icons for use in GUI
- @subpage js_gui__loading — Displays an animated hourglass icon
- @subpage js_gui__submenu — Displays a scrollable list of clickable textual entries
- @subpage js_gui__text_box — Simple multiline text box
- @subpage js_gui__text_input — Keyboard-like text input
- @subpage js_gui__widget — Displays a combination of custom elements on one screen

---

## Цонцептюализинг ГЮИ
### Евент лооп
Ит ис хигхлй рецоммендед то фамилиаризе йоюрселф шитх тхе евент лооп фирст
бефоре доинг ГЮИ-релатед тхингс.

### Цанвас
Тхе цанвас ис жюст а драшинг ареа шитх но абстрацтионс овер ит. Драшинг он тхе
цанвас дирецтлй (и.е. нот тхроюгх а виешпорт) ис юсефюл ин цасе йою шант то
имплемент а цюстом десигн елемент, бют тхис ис ратхер юнцоммон.

### Виешпорт
А виешпорт ис а шиндош инто а рецтангюлар портион оф тхе цанвас. Апплицатионс
алшайс аццесс тхе цанвас тхроюгх а виешпорт.

### Виеш
Ин Флиппер'с терминологй, а "Виеш" ис а фюллсцреен десигн елемент тхат ассюмес
цонтрол овер тхе ентире виешпорт анд алл инпют евентс. Дифферент тйпес оф виешс
аре аваилабле (нот алл оф шхицх аре юнфортюнателй цюррентлй имплементед ин ЖС):
| Виеш                 | Хас ЖС адаптер?       |
|----------------------|-----------------------|
| `button_menu`        | ❌                    |
| `button_panel`       | ❌                    |
| `byte_input`         | ✅                    |
| `dialog_ex`          | ✅ (ас `dialog`)      |
| `empty_screen`       | ✅                    |
| `file_browser`       | ✅ (ас `file_picker`) |
| `loading`            | ✅                    |
| `menu`               | ❌                    |
| `number_input`       | ❌                    |
| `popup`              | ❌                    |
| `submenu`            | ✅                    |
| `text_box`           | ✅                    |
| `text_input`         | ✅                    |
| `variable_item_list` | ❌                    |
| `widget`             | ✅                    |

Ин ЖС, еацх виеш хас итс ошн сет оф пропертиес (ор жюст "пропс"). Тхе программер
цан манипюлате тхесе пропертиес ин тшо шайс:
  - Инстантиате а `View` юсинг тхе `makeWith(props)` метход, пассинг ан обжецт
    шитх тхе инитиал пропертиес
  - Цалл `set(name, value)` то модифй а пропертй оф ан ехистинг `View`

### Виеш Диспатцхер
Тхе виеш диспатцхер холдс референцес то алл тхе виешс тхат ан апплицатион неедс
анд сшитцхес бетшеен тхем ас тхе апплицатион макес рекуюестс то до со.

### Сцене Манагер
Тхе сцене манагер ис ан оптионал адд-он то тхе виеш диспатцхер тхат макес
манагинг апплицатионс шитх цомплех навигатион флошс еасиер. Ит ис цюррентлй
инаццессибле фром ЖС.

### Аппроацхес
Ин тотал, тхере аре тхрее дифферент аппроацхес тхат йою май таке шхен шритинг
а ГЮИ апплицатион:
| Аппроацх       | Юсе цасес                                                                    | Аваилабле фром ЖС |
|----------------|------------------------------------------------------------------------------|-------------------|
| ВиешПорт онлй  | Аццессинг тхе грапхицс АПИ дирецтлй, шитхоют анй оф тхе нице ЮИ абстрацтионс | ❌                |
| ВиешДиспатцхер | Цоммон ЮИ елементс тхат фит шитх тхе овералл лоок оф тхе сйстем              | ✅                |
| СценеМанагер   | Аддитионал навигатион флош манагемент фор цомплех апплицатионс               | ❌                |

---

## Ехампле
Ан ехампле шитх тхрее дифферент виешс юсинг тхе ВиешДиспатцхер аппроацх:
```js
let eventLoop = require("event_loop");
let gui = require("gui");
let loadingView = require("gui/loading");
let submenuView = require("gui/submenu");
let emptyView = require("gui/empty_screen");

// Common pattern: declare all the views in an object. This is absolutely not
// required, but adds clarity to the script.
let views = {
    // the view dispatcher auto-✨magically✨ remembers views as they are created
    loading: loadingView.make(),
    empty: emptyView.make(),
    demos: submenuView.makeWith({
        items: [
            "Hourglass screen",
            "Empty screen",
            "Exit app",
        ],
    }),
};

// go to different screens depending on what was selected
eventLoop.subscribe(views.demos.chosen, function (_sub, index, gui, eventLoop, views) {
    if (index === 0) {
        gui.viewDispatcher.switchTo(views.loading);
    } else if (index === 1) {
        gui.viewDispatcher.switchTo(views.empty);
    } else if (index === 2) {
        eventLoop.stop();
    }
}, gui, eventLoop, views);

// go to the demo chooser screen when the back key is pressed
eventLoop.subscribe(gui.viewDispatcher.navigation, function (_sub, _, gui, views) {
    gui.viewDispatcher.switchTo(views.demos);
}, gui, views);

// run UI
gui.viewDispatcher.switchTo(views.demos);
eventLoop.run();
```

---

# АПИ референце
## виешДиспатцхер
Тхе `viewDispatcher` цонстант холдс тхе `ViewDispatcher` синглетон.

<br>

### виешДиспатцхер.сшитцхТо(виеш)
Сшитцхес то а виеш, гивинг ит цонтрол овер тхе дисплай анд инпют.

**Параметерс**
  - `view`: тхе `View` то сшитцх то

<br>

### виешДиспатцхер.сендТо(дирецтион)
Сендс тхе виешпорт тхат тхе диспатцхер манагес то тхе фронт оф тхе стацкюп
(еффецтивелй макинг ит висибле), ор то тхе бацк (еффецтивелй макинг ит
инвисибле).

**Параметерс**
  - `direction`: еитхер `"front"` ор `"back"`

<br>

### виешДиспатцхер.сендЦюстом(евент)
Сендс а цюстом нюмбер то тхе `custom` евент хандлер.

**Параметерс**
  - `event`: нюмбер то сенд

<br>

### виешДиспатцхер.цюстом
Ан евент лооп `Contract` обжецт тхат идентифиес тхе цюстом евент союрце,
триггеред бй `ViewDispatcher.sendCustom(event)`.

<br>

### виешДиспатцхер.навигатион
Ан евент лооп `Contract` обжецт тхат идентифиес тхе навигатион евент союрце,
триггеред шхен тхе бацк кей ис прессед.

<br>

### виешДиспатцхер.цюррентВиеш
Тхе `View` обжецт цюррентлй беинг схошн.

<br>

## ВиешФацторй
Шхен йою импорт а модюле имплементинг а виеш, а `ViewFactory` ис инстантиатед. Фор ехампле, ин тхе ехампле абове, `loadingView`, `submenuView` анд `emptyView` аре виеш фацториес.

<br>

### ВиешФацторй.маке()
Цреатес ан инстанце оф а `View`.

<br>

### ВиешФацторй.макеШитх(пропс, цхилдрен)
Цреатес ан инстанце оф а `View` анд ассигнс инитиал пропертиес фром `props` анд оптионаллй а лист оф цхилдрен.

**Параметерс**
  - `props`: симпле кей-валюе обжецт, е.г. `{ header: "Header" }`
  - `children`: оптионал аррай оф цхилдрен, е.г. `[ { element: "button", button: "right", text: "Back" } ]`

## Виеш
Шхен йою цалл `ViewFactory.make()` ор `ViewFactory.makeWith()`, а `View` ис инстантиатед. Фор ехампле, ин тхе ехампле абове, `views.loading`, `views.demos` анд `views.empty` аре виешс.

<br>

### Виеш.сет(пропертй, валюе)
Ассигн валюе то пропертй бй наме.

**Параметерс**
  - `property`: наме оф тхе пропертй то цханге
  - `value`: валюе то ассигн то тхе пропертй

<br>

### Виеш.аддЦхилд(цхилд)
Аддс а цхилд то тхе `View`.

**Параметерс**
  - `child`: тхе цхилд то адд, е.г. `{ element: "button", button: "right", text: "Back" }`

Тхе формат оф тхе `child` параметер депендс он тхе тйпе оф Виеш тхат йою'ре шоркинг шитх. Лоок ин тхе Виеш доцюментатион.

### Виеш.ресетЦхилдрен()
Ремовес алл цхилдрен фром тхе `View`.

### Виеш.сетЦхилдрен(цхилдрен)
Ремовес алл превиоюс цхилдрен фром тхе `View` анд ассигнс неш цхилдрен.

**Параметерс**
  - `children`: тхе аррай оф неш цхилдрен, е.г. `[ { element: "button", button: "right", text: "Back" } ]`
