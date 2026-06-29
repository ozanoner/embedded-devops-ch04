
#include "app_bsp.h"
#include "FreeAct.h"
#include "esp_log.h"

#define TAG "app"

typedef struct
{
    Active super;
    TimeEvent te;
    bool green_is_on;
} Blinky;

typedef enum
{
    TIMEOUT_SIG = USER_SIG,
    BUTTON_CLICKED_SIG,
} BlinkySignal_t;

static Blinky blinky;
static StackType_t blinky_stack[configMINIMAL_STACK_SIZE * 2];
static Event *blinky_queue[10];

static void Blinky_ctor(Blinky *const me);
static void Blinky_dispatch(Blinky *const me, Event const *const e);
static void handle_button_click();

void app_main()
{
    ESP_LOGI(TAG, "Example start");
    AppBSP_init();
    AppBSPButton_set_handler(handle_button_click);

    Blinky_ctor(&blinky);
    Active_start(&blinky.super,
                 1, // priority
                 blinky_queue, sizeof(blinky_queue) / sizeof(blinky_queue[0]),
                 blinky_stack, sizeof(blinky_stack),
                 0 // options
    );
}

static void Blinky_ctor(Blinky *const me)
{
    Active_ctor(&me->super, (DispatchHandler)&Blinky_dispatch);
    me->te.type = TYPE_PERIODIC;
    TimeEvent_ctor(&me->te, USER_SIG, &me->super);
    me->green_is_on = false;
}

static void Blinky_dispatch(Blinky *const me, Event const *const e)
{
    ESP_LOGI(TAG, "Blinky_dispatch: sig=%d", e->sig);

    switch (e->sig)
    {
    case INIT_SIG:
        TimeEvent_arm(&me->te, 3000);
        break;

    case TIMEOUT_SIG:
        AppBSP_toggle_red();
        AppBSP_toggle_blue();
        break;

    case BUTTON_CLICKED_SIG:
        AppBSP_toggle_green();
        me->green_is_on = !me->green_is_on;
        ESP_LOGI(TAG, "Green LED: %s", me->green_is_on ? "ON" : "OFF");
        break;

    default:
        break;
    }
}

static void handle_button_click()
{
    ESP_LOGI(TAG, "Button clicked!");

    static Event e = {.sig = BUTTON_CLICKED_SIG};
    Active_post((Active *)&blinky.super, (Event *)&e);
}