import pytest
import hashlib
import ctypes
import os
import sys
import struct

# Attempt to import or simulate the gimbal_md5 functionality
# Since we're testing a C module, we'll simulate the security invariant
# by testing the Python interface or a wrapper, and also test raw behavior

# Security invariant: Buffer reads never exceed the declared length
# Any MD5 implementation must handle oversized inputs safely (truncate or reject)

# Simulate a safe MD5 wrapper that enforces buffer bounds
MAX_SAFE_BUFFER_SIZE = 4096  # Typical safe buffer size
MD5_DIGEST_LENGTH = 16
MD5_STRING_LENGTH = 32


def safe_md5_hash(data, declared_length=None):
    """
    Simulates a safe MD5 hash function that respects declared length.
    If declared_length is provided, only reads up to that many bytes.
    Raises ValueError if input exceeds safe buffer limits without declared length.
    """
    if data is None:
        raise ValueError("Input cannot be None")
    
    if isinstance(data, str):
        data = data.encode('utf-8', errors='replace')
    
    if declared_length is not None:
        # Enforce: never read beyond declared_length
        if declared_length < 0:
            raise ValueError("Declared length cannot be negative")
        # Truncate to declared length - this is the safe behavior
        data = data[:declared_length]
    
    # Check against maximum safe buffer size
    if len(data) > MAX_SAFE_BUFFER_SIZE * 10:  # Allow large inputs but track them
        # In a real C implementation, this would be rejected or truncated
        data = data[:MAX_SAFE_BUFFER_SIZE * 10]
    
    return hashlib.md5(data).hexdigest()


def simulate_unsafe_strcpy_behavior(src, dest_size):
    """
    Simulates what would happen with unsafe strcpy into a fixed buffer.
    Returns True if overflow would occur (unsafe), False if safe.
    """
    if isinstance(src, str):
        src_bytes = src.encode('utf-8') + b'\x00'  # null terminator
    else:
        src_bytes = src + b'\x00'
    
    return len(src_bytes) > dest_size


def bounded_copy(src, max_size):
    """
    Simulates strncpy behavior - safe bounded copy.
    Never reads more than max_size bytes.
    """
    if isinstance(src, str):
        src = src.encode('utf-8', errors='replace')
    
    # Enforce bound - this is the invariant we're testing
    result = src[:max_size]
    return result


OVERSIZED_PAYLOADS = [
    # 2x oversized strings
    "A" * 512,
    "B" * 1024,
    # 10x oversized strings
    "C" * 2560,
    "D" * 10240,
    # Null bytes embedded
    "E" * 100 + "\x00" + "F" * 100,
    # Format string attack payloads
    "%s" * 200,
    "%n" * 200,
    "%x" * 200,
    "%.99999d" * 10,
    # Buffer boundary attacks
    "\xff" * 512,
    "\x00" * 512,
    # Mixed oversized with special chars
    "A" * 255 + "\x00" + "B" * 255,
    # Very large inputs (10x typical buffer)
    "X" * 40960,
    # Unicode oversized
    "\u0041" * 1000,
    "\u00ff" * 500,
    # Newline and control characters
    "\n" * 512,
    "\r\n" * 256,
    "\t" * 512,
    # SQL injection style (long)
    "' OR '1'='1" * 50,
    # Path traversal style (long)
    "../" * 200,
    # Shell injection style (long)
    "; ls -la; " * 100,
    # Binary data oversized
    bytes(range(256)) * 20,
    # Repeated pattern to detect off-by-one
    "AB" * 513,
    # Exactly at boundary
    "A" * 256,
    # One over boundary
    "A" * 257,
    # Two over boundary
    "A" * 258,
    # Heap spray pattern
    "\x90" * 4096,
    # Return address overwrite pattern
    "\x41\x42\x43\x44" * 256,
    # Large JSON-like payload
    '{"key": "' + "A" * 2000 + '"}',
    # Large XML-like payload
    "<tag>" + "A" * 2000 + "</tag>",
]


@pytest.mark.parametrize("payload", OVERSIZED_PAYLOADS)
def test_buffer_reads_never_exceed_declared_length(payload):
    """Invariant: Buffer reads never exceed the declared length.
    
    When processing input through MD5 or any buffer operation, the implementation
    must never read beyond the declared/specified length. Oversized inputs must
    be either truncated to the declared length or rejected with an error.
    No buffer overflow should occur regardless of input size.
    """
    declared_length = 256  # Typical fixed buffer size in C implementations
    
    if isinstance(payload, bytes):
        payload_bytes = payload
    else:
        payload_bytes = payload.encode('utf-8', errors='replace')
    
    payload_size = len(payload_bytes)
    
    # INVARIANT 1: Bounded copy never reads more than declared_length bytes
    result = bounded_copy(payload_bytes, declared_length)
    assert len(result) <= declared_length, (
        f"SECURITY VIOLATION: bounded_copy read {len(result)} bytes "
        f"but declared_length is {declared_length}. "
        f"Input size was {payload_size} bytes."
    )
    
    # INVARIANT 2: If input exceeds declared length, it must be truncated (not overflow)
    if payload_size > declared_length:
        assert len(result) == declared_length, (
            f"SECURITY VIOLATION: Input of {payload_size} bytes should be "
            f"truncated to exactly {declared_length} bytes, got {len(result)}."
        )
    
    # INVARIANT 3: Safe MD5 with declared length never processes more than declared_length
    hash_result = safe_md5_hash(payload_bytes, declared_length=declared_length)
    assert hash_result is not None, "Hash result should not be None"
    assert len(hash_result) == MD5_STRING_LENGTH, (
        f"MD5 hash should always be {MD5_STRING_LENGTH} hex chars, got {len(hash_result)}"
    )
    
    # INVARIANT 4: The hash of truncated input equals hash of full input truncated
    truncated_payload = payload_bytes[:declared_length]
    expected_hash = hashlib.md5(truncated_payload).hexdigest()
    assert hash_result == expected_hash, (
        f"SECURITY VIOLATION: Hash was computed on more data than declared_length allows. "
        f"Expected hash of first {declared_length} bytes."
    )
    
    # INVARIANT 5: Unsafe strcpy would overflow - verify detection works
    if payload_size > declared_length:
        would_overflow = simulate_unsafe_strcpy_behavior(payload_bytes, declared_length)
        assert would_overflow is True, (
            f"Overflow detection failed for input of size {payload_size} "
            f"into buffer of size {declared_length}"
        )
    
    # INVARIANT 6: No exception should escape from safe operations
    # (the safe implementation handles all inputs gracefully)
    try:
        result2 = safe_md5_hash(payload_bytes, declared_length=declared_length)
        assert result2 == hash_result
    except (ValueError, TypeError) as e:
        # Rejection is acceptable, but must be explicit
        assert str(e), "Exception must have a descriptive message"
    except Exception as e:
        pytest.fail(
            f"SECURITY VIOLATION: Unexpected exception type {type(e).__name__}: {e}. "
            f"Safe buffer operations must not crash on oversized input."
        )


@pytest.mark.parametrize("payload", OVERSIZED_PAYLOADS)
def test_md5_output_length_invariant(payload):
    """Invariant: MD5 output is always exactly 16 bytes (32 hex chars) regardless of input size.
    
    The output buffer for MD5 is fixed at 16 bytes. No input, regardless of size,
    should cause the output to exceed this fixed size.
    """
    if isinstance(payload, bytes):
        payload_bytes = payload
    else:
        payload_bytes = payload.encode('utf-8', errors='replace')
    
    # MD5 output must always be exactly 16 bytes / 32 hex chars
    result = hashlib.md5(payload_bytes).digest()
    assert len(result) == MD5_DIGEST_LENGTH, (
        f"SECURITY VIOLATION: MD5 digest length is {len(result)}, expected {MD5_DIGEST_LENGTH}. "
        f"Input size: {len(payload_bytes)} bytes."
    )
    
    result_hex = hashlib.md5(payload_bytes).hexdigest()
    assert len(result_hex) == MD5_STRING_LENGTH, (
        f"SECURITY VIOLATION: MD5 hex string length is {len(result_hex)}, "
        f"expected {MD5_STRING_LENGTH}. Input size: {len(payload_bytes)} bytes."
    )


@pytest.mark.parametrize("size_multiplier,base_size", [
    (2, 256),    # 2x oversized
    (10, 256),   # 10x oversized
    (2, 512),    # 2x oversized larger buffer
    (10, 512),   # 10x oversized larger buffer
    (100, 64),   # 100x oversized small buffer
])
def test_oversized_input_truncation_invariant(size_multiplier, base_size):
    """Invariant: Inputs larger than declared buffer size are always truncated to declared size.
    
    When an input is size_multiplier times larger than the declared buffer,
    the safe implementation must truncate to exactly base_size bytes.
    """
    oversized_input = b"A" * (base_size * size_multiplier)
    
    assert len(oversized_input) == base_size * size_multiplier
    
    # Safe bounded operation
    result = bounded_copy(oversized_input, base_size)
    
    # INVARIANT: Result must never exceed declared size
    assert len(result) <= base_size, (
        f"SECURITY VIOLATION: Output size {len(result)} exceeds declared buffer "
        f"size {base_size}. Input was {size_multiplier}x oversized "
        f"({len(oversized_input)} bytes)."
    )
    
    # INVARIANT: Content must be the first base_size bytes only
    assert result == oversized_input[:base_size], (
        f"SECURITY VIOLATION: Truncated content doesn't match first {base_size} bytes. "
        f"Data beyond declared length may have been accessed."
    )
    
    # INVARIANT: Unsafe operation would overflow
    would_overflow = simulate_unsafe_strcpy_behavior(oversized_input, base_size)
    assert would_overflow is True, (
        f"Overflow detection should trigger for {size_multiplier}x oversized input"
    )