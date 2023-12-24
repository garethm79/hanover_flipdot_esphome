import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, uart
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,  
    CONF_HEIGHT,
    CONF_WIDTH,
    CONF_PAGES,
    CONF_ADDRESS,
)
from esphome.const import __version__ as ESPHOME_VERSION

CODEOWNERS = ["gattrill"]
DEPENDENCIES = ["uart"]

CONF_OFFSET = "offset"

hanover_flipdot_ns = cg.esphome_ns.namespace("hanover_flipdot")
hanover_flipdot = hanover_flipdot_ns.class_(
    "hanover_flipdot", cg.PollingComponent, display.DisplayBuffer, uart.UARTDevice
)


CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(hanover_flipdot),
            cv.Required(CONF_WIDTH, ): cv.positive_int,
            cv.Required(CONF_HEIGHT): cv.positive_int,
            cv.Optional(CONF_ADDRESS, default=0x00): cv.positive_int,
            cv.Optional(CONF_OFFSET, default=0): cv.positive_int,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(cv.polling_component_schema("10s"))
    .extend(uart.UART_DEVICE_SCHEMA),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),

)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await uart.register_uart_device(var, config)
    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_height(config[CONF_HEIGHT]))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_offset(config[CONF_OFFSET]))

    if cv.Version.parse(ESPHOME_VERSION) < cv.Version.parse("2023.12.0"):
        await cg.register_component(var, config)
    await display.register_display(var, config)

    if CONF_OFFSET in config:
        cg.add(var.set_offset(config[CONF_OFFSET]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
