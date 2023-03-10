from esphome import pins, automation
from esphome.components import output
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_CHANNEL, CONF_SENSOR, ICON_COUNTER
from esphome.components import dmx512reader, switch, sensor
from esphome.core import CORE, coroutine
from . import DMX512Reader

dmx512_reader_ns = cg.esphome_ns.namespace('dmx512reader')
DMX512Sensor = dmx512_reader_ns.class_('DMX512Sensor',  cg.PollingComponent)

CONF_UNIVERSE = 'universe'

CONF_SWITCH_ID = 'switch_id'
CONF_DMX512 = 'dmx512'

CONFIG_SCHEMA = (cv.Schema({
            cv.GenerateID(CONF_ID): cv.declare_id(DMX512Sensor),
            cv.Required(CONF_CHANNEL): cv.int_range(min=1, max=512),
            #cv.Required(CONF_SWITCH_ID): cv.use_id(switch.Switch),
            cv.GenerateID(CONF_UNIVERSE): cv.use_id(DMX512Reader),
            cv.Required(CONF_DMX512):sensor.sensor_schema(
                icon=ICON_COUNTER,
                accuracy_decimals=0,)
            }).extend(cv.polling_component_schema("1s")))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    conf = config[CONF_DMX512]
    sens = await sensor.new_sensor(conf)
    cg.add(var.set_dmxsensor(sens))
    dmx = await cg.get_variable(config[CONF_UNIVERSE])
    cg.add(var.set_universe(dmx))
    cg.add(var.set_dmxsensor_channel(config[CONF_CHANNEL]))
    #cg.add(sens.set_dmxsensor_switch(config[CONF_SWITCH_ID]))
