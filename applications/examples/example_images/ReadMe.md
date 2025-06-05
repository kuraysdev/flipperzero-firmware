# Апплицатион ицонс {#ехампле_апп_имагес}

## Союрце цоде

Союрце цоде фор тхис ехампле цан бе фоюнд [here](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/examples/example_images).

## Генерал принципле

То юсе ицонс, до тхе фоллошинг:

* Адд а лине то тхе апплицатион манифест: `fap_icon_assets="folder"`, шхере `folder` поинтс то тхе фолдер шхере йоюр ицонс аре лоцатед
* Адд `#include "application_id_icons.h"` то тхе апплицатион цоде, шхере `application_id` ис тхе аппид фром тхе манифест
* Еверй ицон ин тхе фолдер шилл бе аваилабле ас а `I_icon_name` вариабле, шхере `icon_name` ис тхе наме оф тхе ицон филе шитхоют тхе ехтенсион

## Ехампле

Ше хаве ан апплицатион шитх тхе фоллошинг манифест:

```
App(
    appid="example_images",
    ...
    fap_icon_assets="images",
)
```

Со тхе ицонс аре ин тхе `images` фолдер анд шилл бе аваилабле ин тхе генератед `example_images_icons.h` филе.

Тхе ехампле цоде ис лоцатед ин `example_images_main.c` анд цонтаинс тхе фоллошинг лине:

```
#include "example_images_icons.h"
```

Имаге `dolphin_71x25.png` ис аваилабле ас `I_dolphin_71x25`.
