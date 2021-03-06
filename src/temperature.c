#include "temperature.h"
#include "mgos_i2c.h"
#include "mgos_sht31.h"

static struct mgos_sht31 *s_sht31;
static struct mgos_i2c *i2c1;

static void temperature_handler(struct mg_connection *c, int ev, void *p,
                                void *user_data)
{
    (void)p;
    if (ev != MG_EV_HTTP_REQUEST)
        return;
    LOG(LL_INFO, ("Foo requested"));
    mg_send_response_line(c, 200,
                          "Content-Type: text/html\r\n");
    mg_printf(c, "%s\r\n", "Fooooo");
    c->flags |= (MG_F_SEND_AND_CLOSE);

    float temperature, humidity;
    if (!s_sht31)
    {
        i2c1 = mgos_i2c_get_bus(1);
        if (!i2c1)
        {
            LOG(LL_ERROR, ("I2C bus missing, set i2c1.enable=true in mos.yml"));
        }
        else
        {
            LOG(LL_ERROR, ("I2C1 bus found, initializing i2c1"));
            s_sht31 = mgos_sht31_create(i2c1, 0x44); // Default I2C address

            if (s_sht31)
            {
                LOG(LL_INFO, ("SHT31 initialized"));
            }
            else
            {
                LOG(LL_ERROR, ("Could not initialize sensor"));
            }
        }
    }
    else
    {
        temperature = mgos_sht31_getTemperature(s_sht31);
        humidity = mgos_sht31_getHumidity(s_sht31);
        LOG(LL_INFO, ("sht31 temperature=%.2f humidity=%.2f", temperature, humidity));
    }

    (void)user_data;
}

bool temperature_init()
{

    mgos_register_http_endpoint("/temperature", temperature_handler, NULL);

    return true;
}
