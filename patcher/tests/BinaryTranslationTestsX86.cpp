#include <patcher/binary_translation.h>

#include "helpers.h"

#include <ut/assert.h>
#include <ut/test.h>

using namespace std;

namespace micro_profiler
{
	namespace tests
	{
		begin_test_suite( BinaryTranslationTestsX86 )

			// Terminology:
			//		- 'inner' jump/call is made within the bounds of a isolated function piece;
			//		- 'external' jump/call goes beyond the bounds of a isolated piece;
			//		- 'short' jump/call is a rel8 (Intel) instruction;
			//		- 'near' jump/call is a rel32 (Intel) instruction;

			test( RelativeExternalJumpsAreTranslatedBasedOnTheirTargetAddress )
			{
				// INIT
				byte instructions1[] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xE9, 0xF0, 0xFF, 0xFF, 0xFF, 0xE9, 0x00, 0x00, 0x10, 0x00,
				};

				// ACT
				move_function(instructions1 + 1, const_byte_range(instructions1 + 48, 10));

				// ASSERT
				byte reference1[] = {
					0xE9, 0x1F, 0x00, 0x00, 0x00, 0xE9, 0x2F, 0x00, 0x10, 0x00,
				};

				assert_is_true(equal(reference1, array_end(reference1), instructions1 + 1));

				// ACT
				move_function(instructions1 + 17, const_byte_range(instructions1 + 48, 10));

				// ASSERT
				byte reference2[] = {
					0xE9, 0x0F, 0x00, 0x00, 0x00, 0xE9, 0x1F, 0x00, 0x10, 0x00,
				};

				assert_is_true(equal(reference2, array_end(reference2), instructions1 + 17));

				// INIT
				byte instructions2[0x1000] = {
					0xE9, 0x21, 0x10, 0x00, 0x10, 0xE9, 0x00, 0xFF, 0xFF, 0xFF, 0xE9, 0xFF, 0x00, 0x00, 0x00,
				};
		
				// ACT
				move_function(instructions2 + 0x0F12, const_byte_range(instructions2, 15));

				// ASSERT
				byte reference3[] = {
					0xE9, 0x0F, 0x01, 0x00, 0x10, 0xE9, 0xEE, 0xEF, 0xFF, 0xFF, 0xE9, 0xED, 0xF1, 0xFF, 0xFF,
				};

				assert_is_true(equal(reference3, array_end(reference3), instructions2 + 0x0F12));
			}


			test( RelativeExternalCallsAreTranslatedBasedOnTheirTargetAddress )
			{
				// INIT
				byte instructions1[] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xE8, 0xF0, 0xFF, 0xFF, 0xFF, 0xE8, 0x00, 0x00, 0x10, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00,
				};

				// ACT
				move_function(instructions1 + 1, const_byte_range(instructions1 + 48, 15));

				// ASSERT
				byte reference1[] = {
					0xE8, 0x1F, 0x00, 0x00, 0x00, 0xE8, 0x2F, 0x00, 0x10, 0x00, 0xE8, 0x2F, 0x00, 0x00, 0x00,
				};

				assert_is_true(equal(reference1, array_end(reference1), instructions1 + 1));

				// ACT
				move_function(instructions1 + 17, const_byte_range(instructions1 + 48, 15));

				// ASSERT
				byte reference2[] = {
					0xE8, 0x0F, 0x00, 0x00, 0x00, 0xE8, 0x1F, 0x00, 0x10, 0x00, 0xE8, 0x1F, 0x00, 0x00, 0x00,
				};

				assert_is_true(equal(reference2, array_end(reference2), instructions1 + 17));

				// INIT
				byte instructions2[0x1000] = {
					0xE8, 0x21, 0x10, 0x00, 0x10, 0xE8, 0x00, 0xFF, 0xFF, 0xFF, 0xE8, 0xFF, 0x00, 0x00, 0x00,
					0xE8, 0x00, 0x00, 0x00, 0x00,
				};
		
				// ACT
				move_function(instructions2 + 0x0F11, const_byte_range(instructions2, 20));

				// ASSERT
				byte reference3[] = {
					0xE8, 0x10, 0x01, 0x00, 0x10, 0xE8, 0xEF, 0xEF, 0xFF, 0xFF, 0xE8, 0xEE, 0xF1, 0xFF, 0xFF,
					0xE8, 0xEF, 0xF0, 0xFF, 0xFF,
				};

				assert_is_true(equal(reference3, array_end(reference3), instructions2 + 0x0F11));
			}


			test( RelativeInnerJumpsAreNotTranslatedWhenMoved )
			{
				// INIT
				byte instructions[0x100] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xE9, 0x05, 0x00, 0x00, 0x00, 0xE9, 0x00, 0x00, 0x00, 0x00, 0xE9, 0xF1, 0xFF, 0xFF, 0xFF,
					0xE9, 0xF6, 0xFF, 0xFF, 0xFF,
				};

				// ACT
				move_function(instructions, const_byte_range(instructions + 0x20, 20));
				move_function(instructions + 0x40, const_byte_range(instructions + 0x20, 20));

				// ASSERT
				byte reference[] = {
					0xE9, 0x05, 0x00, 0x00, 0x00, 0xE9, 0x00, 0x00, 0x00, 0x00, 0xE9, 0xF1, 0xFF, 0xFF, 0xFF,
					0xE9, 0xF6, 0xFF, 0xFF, 0xFF
				};

				assert_is_true(equal(reference, array_end(reference), instructions));
				assert_is_true(equal(reference, array_end(reference), instructions + 0x40));
			}


			test( RelativeInnerCallsAreNotTranslatedWhenMoved )
			{
				// INIT
				byte instructions[0x100] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xE8, 0x05, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xF6, 0xFF, 0xFF, 0xFF,
					0xE8, 0xF6, 0xFF, 0xFF, 0xFF,
				};

				// ACT
				move_function(instructions, const_byte_range(instructions + 0x20, 20));
				move_function(instructions + 0x40, const_byte_range(instructions + 0x20, 20));

				// ASSERT
				byte reference[] = {
					0xE8, 0x05, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xE8, 0xF6, 0xFF, 0xFF, 0xFF,
					0xE8, 0xF6, 0xFF, 0xFF, 0xFF,
				};

				assert_is_true(equal(reference, array_end(reference), instructions));
				assert_is_true(equal(reference, array_end(reference), instructions + 0x40));
			}


			test( InstructionMixWithExternalJumpsAndCallsIsTranslatedToTargetAddress )
			{
				// INIT
				byte instructions[0x400] = {
					0xE8, 0xD4, 0x54, 0x02, 0x00,			// call memset
					0x83, 0xC4, 0x0C,							// add esp, 0Ch
					0x6A, 0x0F,									// push 0Fh
					0x8D, 0x8D, 0xB4, 0xEF, 0xFF, 0xFF,	// lea ecx, [instructions2]
					0x51,											// push ecx
					0x8D, 0x95, 0xC5, 0xFE, 0xFF, 0xFF,	// lea edx, [ebp-13Bh]
					0x52,											// push edx
					0xE8, 0x8C, 0x98, 0x01, 0x00,			// call micro_profiler::move_function
					0x83, 0xC4, 0x0C,							// add esp, 0Ch
					0xE9, 0x12, 0x34, 0x56, 0x78,			// jmp somewhere...
				};
				byte *translated = instructions + 0x0123;

				// ACT
				move_function(translated, const_byte_range(instructions, 0x0025));

				// ASSERT
				byte reference1[] = {
					0xE8, 0xB1, 0x53, 0x02, 0x00,
					0x83, 0xC4, 0x0C,
					0x6A, 0x0F,
					0x8D, 0x8D, 0xB4, 0xEF, 0xFF, 0xFF,
					0x51,
					0x8D, 0x95, 0xC5, 0xFE, 0xFF, 0xFF,
					0x52,
					0xE8, 0x69, 0x97, 0x01, 0x00,
					0x83, 0xC4, 0x0C,
					0xE9, 0xEF, 0x32, 0x56, 0x78,
				};

				assert_is_true(equal(reference1, array_end(reference1), translated));
			}


			test( CalculateLengthReturnsSameLengthOnEvenInstructionBoundaries )
			{
				// INIT
				byte instructions[] = {
					0xE8, 0xD4, 0x54, 0x02, 0x00,			// call memset
					0x83, 0xC4, 0x0C,							// add esp, 0Ch
					0x6A, 0x0F,									// push 0Fh
					0x8D, 0x8D, 0xB4, 0xEF, 0xFF, 0xFF,	// lea ecx, [instructions2]
					0x51,											// push ecx
					0x8D, 0x95, 0xC5, 0xFE, 0xFF, 0xFF,	// lea edx, [ebp-13Bh]
					0x52,											// push edx
					0xE8, 0x8C, 0x98, 0x01, 0x00,			// call micro_profiler::move_function
					0x83, 0xC4, 0x0C,							// add esp, 0Ch
					0xE9, 0x12, 0x34, 0x56, 0x78,			// jmp somewhere...
				};

				// ACT / ASSERT
				assert_equal(5u, calculate_fragment_length(mkrange(instructions), 5));
				assert_equal(3u, calculate_fragment_length(const_byte_range(instructions + 5, 3), 3));
				assert_equal(2u, calculate_fragment_length(const_byte_range(instructions + 8, 2), 2));
				assert_equal(8u, calculate_fragment_length(mkrange(instructions), 8));
				assert_equal(11u, calculate_fragment_length(const_byte_range(instructions + 5, sizeof(instructions) - 5), 11));
			}


			test( CalculateLengthReturnsCorrectLengthOnUnevenInstructionBoundaries )
			{
				// INIT
				byte instructions[] = {
					0xE8, 0xD4, 0x54, 0x02, 0x00,			// call memset
					0x83, 0xC4, 0x0C,							// add esp, 0Ch
					0x6A, 0x0F,									// push 0Fh
					0x8D, 0x8D, 0xB4, 0xEF, 0xFF, 0xFF,	// lea ecx, [instructions2]
					0x51,											// push ecx
					0x8D, 0x95, 0xC5, 0xFE, 0xFF, 0xFF,	// lea edx, [ebp-13Bh]
					0x52,											// push edx
					0xE8, 0x8C, 0x98, 0x01, 0x00,			// call micro_profiler::move_function
					0x83, 0xC4, 0x0C,							// add esp, 0Ch
					0xE9, 0x12, 0x34, 0x56, 0x78,			// jmp somewhere...
				};

				// ACT / ASSERT
				assert_equal(5u, calculate_fragment_length(mkrange(instructions), 2));
				assert_equal(3u, calculate_fragment_length(const_byte_range(instructions + 5, 3), 1));
				assert_equal(8u, calculate_fragment_length(const_byte_range(instructions + 8, 8), 3));
				assert_equal(8u, calculate_fragment_length(mkrange(instructions), 8));
				assert_equal(7u, calculate_fragment_length(const_byte_range(instructions + 16, sizeof(instructions) - 16), 4));
			}


			test( ImagesWithExternalShortJumpsCannotBeMoved )
			{
				// INIT
				byte instructions[0x0400] = {
					0xEB, 0x02,
					0xEB, 0x00,
					0xEB, 0xF9,
					0xEB, 0xF5,
				};

				// ACT / ASSERT
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions, 2)),
					inconsistent_function_range_exception);
			}


			test( ImagesWithExternalShortConditionalJumpsCannotBeMoved )
			{
				// INIT
				byte instructions[0x0400] = {
					0x70, 0x1E, 0x71, 0x1C, 0x72, 0x1A, 0x73, 0x18, 0x74, 0x16, 0x75, 0x14, 0x76, 0x12, 0x77, 0x10,
					0x78, 0x0E, 0x79, 0x0C, 0x7A, 0x0A, 0x7B, 0x08, 0x7C, 0x06, 0x7D, 0x04, 0x7E, 0x02, 0x7F, 0x00,
				};

				// ACT / ASSERT
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 2, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 4, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 6, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 8, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 10, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 12, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 14, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 16, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 18, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 20, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 22, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 24, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 26, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 28, 2)),
					inconsistent_function_range_exception);
				assert_throws(move_function(instructions + 0x30, const_byte_range(instructions + 30, 2)),
					inconsistent_function_range_exception);
			}


			test( ImagesWithInnerShortJumpsCanBeMoved )
			{
				// INIT
				byte instructions[0x0400] = {
					0xEB, 0x02,
					0xEB, 0x00,
					0xEB, 0xFA,
				};

				// ACT
				move_function(instructions + 0x30, const_byte_range(instructions, 6));

				// ASSERT
				assert_equal(const_byte_range(instructions, 6), const_byte_range(instructions + 0x30, 6));
			}


			test( ImagesWithInnerShortConditionalJumpsCanBeMoved )
			{
				// INIT
				byte instructions[0x0400] = {
					0x70, 0x1E, 0x71, 0x1C, 0x72, 0x1A, 0x73, 0x18, 0x74, 0x16, 0x75, 0x14, 0x76, 0x12, 0x77, 0x10,
					0x78, 0x0E, 0x79, 0x0C, 0x7A, 0x0A, 0x7B, 0x08, 0x7C, 0x06, 0x7D, 0x04, 0x7E, 0x02, 0x7F, 0x00,
					0x90,
				};

				// ACT
				move_function(instructions + 0x30, const_byte_range(instructions, 0x21));

				// ASSERT
				assert_equal(const_byte_range(instructions, 0x21), const_byte_range(instructions + 0x30, 0x21));
			}


			test( SSEInstructionsAreProperlyRead )
			{
				// INIT
				byte is1[] = { 0x66, 0x0F, 0x6C, 0xC0, 0xCC, };
				byte is2[] = { 0x66, 0x0F, 0x7F, 0x44, 0x24, 0x3C, 0xCC, 0xCC, };
				byte is3[] = { 0xF2, 0x0F, 0x70, 0x3C, 0x69, 0x05, }; // pshuflw	xmm7,xmmword ptr [ecx+ebp*2],5
				byte is4[] = { 0x66, 0x0F, 0x71, 0xD0, 0x05, };
				byte is5[] = { 0x0F, 0xC7, 0x09, }; //	cmpxchg8b	qword ptr [ecx]
				byte is6[] = { 0x0F, 0xC7, 0x8C, 0xA9, 0x00, 0x00, 0x10, 0x00, };	//	cmpxchg8b	qword ptr [ecx+ebp*4+100000h]

				// ACT / ASSERT
				assert_equal(4u, calculate_fragment_length(mkrange(is1), 1));
				assert_equal(6u, calculate_fragment_length(mkrange(is2), 1));
				assert_equal(6u, calculate_fragment_length(mkrange(is3), 1));
				assert_equal(5u, calculate_fragment_length(mkrange(is4), 1));
				assert_equal(3u, calculate_fragment_length(mkrange(is5), 1));
				assert_equal(8u, calculate_fragment_length(mkrange(is6), 1));
			}


			test( RelativeNearExternalConditionalJumpsAreTranslatedBasedOnTheirTargetAddress )
			{
				// INIT
				byte is1[0x400] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xEB, 0x00, 0x0F, 0Xff, 0x13, 0x45, 0x00, 0x10, 0xEB, 0x00, 0xEB, 0xFC,
				};
				byte is2[0x400] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xEB, 0x00, 0xEB, 0x00, 0x0F, 0Xff, 0x13, 0x45, 0x00, 0xF0, 0xEB, 0x00, 0xEB, 0xFC,
				};

				for (byte i = 0x80; i != 0x90; ++i)
				{
					is1[0x43] = i;
					is2[0x35] = i;

				// ACT
					move_function(is1 + 0x1F0, const_byte_range(is1 + 0x40, 0x0C));
					move_function(is1 + 0x01, const_byte_range(is1 + 0x40, 0x0C));
					move_function(is2 + 0x2E7, const_byte_range(is2 + 0x30, 0x0E));
					move_function(is2 + 0x03, const_byte_range(is2 + 0x30, 0x0E));

				// ASSERT
					byte reference1[] = { 0xEB, 0x00, 0x0F, i, 0x63, 0x43, 0x00, 0x10, 0xEB, 0x00, 0xEB, 0xFC, };
					byte reference2[] = { 0xEB, 0x00, 0x0F, i, 0x52, 0x45, 0x00, 0x10, 0xEB, 0x00, 0xEB, 0xFC, };
					byte reference3[] = { 0xEB, 0x00, 0xEB, 0x00, 0x0F, i, 0x5C, 0x42, 0x00, 0xF0, 0xEB, 0x00, 0xEB, 0xFC, };
					byte reference4[] = { 0xEB, 0x00, 0xEB, 0x00, 0x0F, i, 0x40, 0x45, 0x00, 0xF0, 0xEB, 0x00, 0xEB, 0xFC, };

					assert_equal(reference1, const_byte_range(is1 + 0x1F0, 0x0C));
					assert_equal(reference2, const_byte_range(is1 + 0x01, 0x0C));
					assert_equal(reference3, const_byte_range(is2 + 0x2E7, 0x0E));
					assert_equal(reference4, const_byte_range(is2 + 0x03, 0x0E));
				}
			}


			test( RelativeNearInnerConditionalJumpsAreNotTranslated )
			{
				// INIT
				byte is1[0x400] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xEB, 0x00, 0x0F, 0Xff, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x00, 0xEB, 0xFC,
				};
				byte is2[0x400] = {
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0xEB, 0x00, 0xEB, 0x00, 0x0F, 0Xff, 0xF6, 0xFF, 0xFF, 0xFF, 0xEB, 0x00, 0xEB, 0xFC,
				};

				for (byte i = 0x80; i != 0x90; ++i)
				{
					is1[0x43] = i;
					is2[0x35] = i;

				// ACT
					move_function(is1 + 0x1F0, const_byte_range(is1 + 0x40, 0x0C));
					move_function(is1 + 0x01, const_byte_range(is1 + 0x40, 0x0C));
					move_function(is2 + 0x2E7, const_byte_range(is2 + 0x30, 0x0E));
					move_function(is2 + 0x03, const_byte_range(is2 + 0x30, 0x0E));

				// ASSERT
					byte reference1[] = { 0xEB, 0x00, 0x0F, i, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x00, 0xEB, 0xFC, };
					byte reference2[] = { 0xEB, 0x00, 0xEB, 0x00, 0x0F, i, 0xF6, 0xFF, 0xFF, 0xFF, 0xEB, 0x00, 0xEB, 0xFC, };

					assert_equal(reference1, const_byte_range(is1 + 0x1F0, 0x0C));
					assert_equal(reference1, const_byte_range(is1 + 0x01, 0x0C));
					assert_equal(reference2, const_byte_range(is2 + 0x2E7, 0x0E));
					assert_equal(reference2, const_byte_range(is2 + 0x03, 0x0E));
				}
			}


			test( InstructionStreamWithInterruptsCannotBeMoved )
			{
				// INIT
				byte is1[0x100] = { 0xCC, 0xEB, 0x00, 0xEB, 0xFC, };
				byte is2[0x100] = { 0xEB, 0x00, 0xCC, 0xEB, 0xFB, };

				// ACT / ASSERT
				assert_throws(move_function(is1 + 0x51, const_byte_range(is1, 5)), inconsistent_function_range_exception);
				assert_throws(move_function(is2 + 0x50, const_byte_range(is2, 5)), inconsistent_function_range_exception);
			}

		end_test_suite
	}
}
