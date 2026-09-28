# ARES UART peer protocol draft v0.1

## Physical layer
Start with short 3.3 V full-duplex UART: separate TX/RX and common ground. Confirm exact board pin tolerances. For long/noisy wiring, evaluate a suitable differential transceiver and test EMI; a logic buffer alone does not solve long-cable noise.

## Equal peers and collisions
Separate TX/RX wires allow simultaneous transmission without electrical bus contention. Application-level ordering, duplicate handling and delivery confirmation still require protocol rules. Each node may publish independently. Critical unicast messages use ACK, timeout and bounded retries.

## Frame
SYNC | VERSION | SRC | DST | TYPE | SEQ | LENGTH | PAYLOAD | CRC16
- Multibyte fields little-endian.
- SEQ is a per-source 16-bit counter.
- DST 0xFF is broadcast; no ACK for broadcast.
- CRC16-CCITT-FALSE, polynomial 0x1021, init 0xFFFF; excludes SYNC.
- Initial maximum payload 128 bytes; revise after throughput measurements.
- Reject invalid length, CRC or version; count and log errors.

Message types: HELLO, HEARTBEAT, SENSOR_DATA, STATUS, ACK, NACK, TIME_SYNC, CONFIG. ACK/NACK references the source and sequence. Duplicate SRC+SEQ must be idempotent.

Use separate TX/RX ring buffers. Prioritize critical alerts; aggregate or decimate routine telemetry. Test loopback, simultaneous traffic, corruption, truncation, reboot, overload, cable extension/EMI and power loss.
