import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, i2c
from esphome.const import CONF_INTERLOCK, CONF_ID, CONF_NAME, CONF_ASSUMED_STATE

DEPENDENCIES = ["i2c"]

CONF_INTERLOCK_WAIT_TIME = "interlock_wait_time"

M5RELAY4_ns = cg.esphome_ns.namespace("m5stack4relay")
M5Relay4Control = M5RELAY4_ns.class_("M5Relay4Control", cg.Component, i2c.I2CDevice)
M5Relay4Switch = M5RELAY4_ns.class_("M5Relay4Switch", switch.Switch, cg.Component)

CONF_RELAY1 = "relay1"
CONF_RELAY2 = "relay2"
CONF_RELAY3 = "relay3"
CONF_RELAY4 = "relay4"

CONF_LED1 = "led1"
CONF_LED2 = "led2"
CONF_LED3 = "led3"
CONF_LED4 = "led4"

CONF_SYNC_MODE = "sync_mode"

RelayBit_ = M5RELAY4_ns.enum("RelayBit", is_class=True)
ENUM_COMP_SWITCHES = {
    CONF_LED1: RelayBit_.LED1,
    CONF_LED2: RelayBit_.LED2,
    CONF_LED3: RelayBit_.LED3,
    CONF_LED4: RelayBit_.LED4,
    CONF_RELAY1: RelayBit_.RELAY1,
    CONF_RELAY2: RelayBit_.RELAY2,
    CONF_RELAY3: RelayBit_.RELAY3,
    CONF_RELAY4: RelayBit_.RELAY4,
}


def check_relayswitch():
    return switch.switch_schema(M5Relay4Switch).extend(
        {
                cv.Optional(CONF_INTERLOCK): cv.ensure_list(cv.use_id(switch.Switch)),
                cv.Optional(CONF_INTERLOCK_WAIT_TIME, default="0ms"): cv.positive_time_period_milliseconds,
        }
        )


CONFIG_SCHEMA = (cv.Schema({
            cv.GenerateID(): cv.declare_id(M5Relay4Control),
            cv.Optional(CONF_SYNC_MODE): cv.boolean,
            cv.Optional(CONF_RELAY1): check_relayswitch(),
            cv.Optional(CONF_RELAY2): check_relayswitch(),
            cv.Optional(CONF_RELAY3): check_relayswitch(),
            cv.Optional(CONF_RELAY4): check_relayswitch(),
            cv.Optional(CONF_LED1): check_relayswitch(),
            cv.Optional(CONF_LED2): check_relayswitch(),
            cv.Optional(CONF_LED3): check_relayswitch(),
            cv.Optional(CONF_LED4): check_relayswitch(),
        })
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x26)))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_SYNC_MODE in config:
        cg.add(var.set_sync_mode(config[CONF_SYNC_MODE]))

    for key, value in ENUM_COMP_SWITCHES.items():
        if key in config:
            conf = config[key]
            swi = await switch.new_switch(conf, value)
            
            if CONF_INTERLOCK in conf:
                interlock = []
                for it in conf[CONF_INTERLOCK]:
                    lock = await cg.get_variable(it)
                    interlock.append(lock)
                cg.add(swi.set_interlock(interlock))
                cg.add(swi.set_interlock_wait_time(conf[CONF_INTERLOCK_WAIT_TIME]))
            cg.add(swi.set_parent(var))
