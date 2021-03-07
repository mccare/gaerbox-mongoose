#include "temperature.h"
#include "mgos_i2c.h"
#include "mgos_sht30.h"
#include <math.h>

static struct mgos_sht30 *s_sht30;
static struct mgos_i2c *i2c;
static bool heating;

static void temperature_handler(struct mg_connection *c, int ev, void *p,
                                void *user_data)
{
    (void)p;
    if (ev != MG_EV_HTTP_REQUEST)
        return;
    LOG(LL_INFO, ("Foo requested"));
    mg_send_response_line(c, 200,
                          "Content-Type: application/json\r\n");

    float temperature, humidity;
    temperature = -100.0;
    humidity = 0.0;

    temperature = mgos_sht30_getTemperature(s_sht30);
    humidity = mgos_sht30_getHumidity(s_sht30);
    LOG(LL_INFO, ("sht30 temperature=%.2f humidity=%.2f", temperature, humidity));

    mg_printf(c, "{ \"temperature\": %.2f, \"humidity\": %.2f, \"target_temperature\": %d, \"heating\": %d }",
              temperature, humidity, mgos_sys_config_get_app_target_temperature(), heating);
    c->flags |= (MG_F_SEND_AND_CLOSE);

    (void)user_data;
}

static void temperature_timer_cb(void *arg)
{
    float temperature = mgos_sht30_getTemperature(s_sht30);

    LOG(LL_INFO,
        ("Temperature %.2f target_temperature: %d heating: %d", temperature, mgos_sys_config_get_app_target_temperature(), heating));
    if (fabs(temperature - mgos_sys_config_get_app_target_temperature()) > (mgos_sys_config_get_app_target_temperature_delta() / 10.0))
    {
        if (temperature > mgos_sys_config_get_app_target_temperature())
        {
            LOG(LL_INFO,
                ("Switching off heating!"));
            heating = false;
        }
        else
        {
            LOG(LL_INFO,
                ("Switching on heating!"));
            heating = true;
        }
        mgos_gpio_write(mgos_sys_config_get_app_relay_pin(), heating);
    }
    (void)arg;
}

bool temperature_init()
{
    mgos_register_http_endpoint("/temperature", temperature_handler, NULL);
    mgos_set_timer(3000 /* ms */, MGOS_TIMER_REPEAT, temperature_timer_cb, NULL);
    mgos_gpio_setup_output(mgos_sys_config_get_app_relay_pin(), false);

    i2c = mgos_i2c_get_bus(0);
    if (!i2c)
    {
        LOG(LL_ERROR, ("I2C bus missing, set i2c.enable=true in mos.yml"));
    }
    else
    {
        LOG(LL_ERROR, ("i2c bus found, initializing i2c"));
        s_sht30 = mgos_sht30_create(i2c, 0x44); // Default I2C address

        if (s_sht30)
        {
            LOG(LL_INFO, ("sht30 initialized"));
        }
        else
        {
            LOG(LL_ERROR, ("Could not initialize sensor"));
        }
    }

    return true;
}
