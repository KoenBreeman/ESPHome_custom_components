import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome import automation

from esphome.const import (
    CONF_ID,
    CONF_INVERTED,
    CONF_LEVEL,
    CONF_MAX_LEVEL,
    CONF_CHANNEL
)
from esphome.core import CORE

DEPENDENCIES = ["i2c"]
MULTI_CONF = True

CODEOWNERS = ["@Koen Breeman"]

GP8403_ns = cg.esphome_ns.namespace("GP8403")
GP8403 = GP8403_ns.class_("GP8403", cg.Component)

CONF_I2C_ADDR = 0x5F#0X26

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(GP8403),
        cv.Required(CONF_MAX_LEVEL): cv.int_range(min=0, max=10),

    }, extra=cv.ALLOW_EXTRA,
).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(CONF_I2C_ADDR))

async def to_code(config):
    cg.add_global(GP8403_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_OutVoltage(config[CONF_MAX_LEVEL]))
    #cg.add(var.set_channel(config[CONF_CHANNEL]))
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

