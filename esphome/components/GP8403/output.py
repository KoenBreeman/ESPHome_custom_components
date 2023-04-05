import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_CHANNEL, CONF_ID
from . import GP8403, GP8403_ns

DEPENDENCIES = ["GP8403"]

GP8403Output = GP8403_ns.class_("GP8403Output", output.FloatOutput)
CONF_GP8403_ID = "GP8403_id"

<<<<<<< HEAD
CONF_OUTPUT_SIN = "output_sin"
CONF_OUTPUT_SQUARE = "output_square"
CONF_OUTPUT_TRIANGLE = "output_triangle"

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend({
        cv.GenerateID(CONF_GP8403_ID): cv.use_id(GP8403),
        cv.GenerateID(CONF_ID): cv.declare_id(GP8403Output),
        cv.Required(CONF_CHANNEL):cv.int_range(min=0, max=2),
        #cv.Optional(CONF_OUTPUT_SIN,default=False):cv.boolean,
        #cv.Optional(CONF_OUTPUT_SQUARE,default=False):cv.boolean,
        #cv.Optional(CONF_OUTPUT_TRIANGLE,default=False):cv.boolean,
    }).extend(cv.COMPONENT_SCHEMA)
=======
CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_GP8403_ID): cv.use_id(GP8403),
        cv.Required(CONF_ID): cv.declare_id(GP8403Output),
    }
).extend(cv.COMPONENT_SCHEMA)
>>>>>>> parent of 525c1df (Update output en init.py so that also channel can be set in the output. Also included a test.yaml)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await output.register_output(var, config)

    paren = await cg.get_variable(config[CONF_GP8403_ID])
    cg.add(var.set_parent(paren))

    #cg.add(var.set_output_sin(config[CONF_OUTPUT_SIN]))
    #cg.add(var.set_output_square(config[CONF_OUTPUT_SQUARE]))
    #cg.add(var.set_output_triangle(config[CONF_OUTPUT_TRIANGLE]))

