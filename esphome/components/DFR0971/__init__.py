import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.automation import maybe_simple_id
from esphome.const import (
    CONF_ID,
    CONF_INVERTED,
    CONF_LEVEL,
    CONF_MAX_POWER,
    CONF_MIN_POWER,
)
from esphome.core import CORE

CODEOWNERS = ["@Koen Breeman"]

DFR0971_ns = cg.esphome_ns.namespace("DFR0971")
DFR0971 = DFR0971_ns.class_("DFR0971", cg.Component)
Channel = DFR0971.class_("Channel", output.FloatOutput)

IS_PLATFORM_COMPONENT = True

CONF_ZERO_MEANS_ZERO = "zero_means_zero"
CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(BP5758D),
        cv.Required(CONF_MAX_POWER): cv.int_range(min=0, max=10),
    }
).extend(cv.COMPONENT_SCHEMA)

