
// default is:
//  - undef ROLLED         
//  - define VOLUMERAMPING 
// see the Makefile...

.data

mix_numsamples:
	.long	0
mix_mixptr:
	.long	0
mix_mixbuffend:
	.long	0
mix_mixbuffptr:
	.long	0
mix_endflag:
	.long	0
mix_sptr:
	.long	0
mix_cptr:
	.long	0
mix_count:
	.long	0
mix_samplebuff:
	.long	0
mix_leftvol:
	.long	0
mix_rightvol:
	.long	0
mix_temp1:
	.long	0
mix_count_old:
	.long	0
mix_rampleftvol:
	.long	0
mix_ramprightvol:
	.long	0
mix_rampcount:
	.long	0
mix_rampspeedleft:
	.long	0
mix_rampspeedright:
	.long	0
_mix_volumerampsteps:
	.long	0
_mix_1overvolumerampsteps:
	.long	0
mix_255:
	.single	255.0
mix_256:
	.single 256.0
mix_1over255:
	.single	0.0039215686	
mix_1over256:
	.single	0.00390625		
mix_1over2gig:
	.single	4.65612e-10		

	.globl _mix_volumerampsteps
	.globl _mix_1overvolumerampsteps

.text

	.globl FSOUND_Mixer_FPU_Ramp_ASM
FSOUND_Mixer_FPU_Ramp_ASM:
	push   %ebp

	mov    %eax,mix_numsamples

	mov    %edi,mix_mixptr
	mov    %edi,mix_mixbuffptr

	mov    %ecx,mix_cptr
	mov    %edx,mix_mixbuffend

	cmp    $0x0,%ecx
	je     MixExit		

	mov    28(%ecx),%ebx	
	mov    %ebx,mix_sptr					

 	cmp    $0x0,%ebx
	je     MixExit		

	mov    (%ebx),%eax
	mov    %eax,mix_samplebuff

	mov    mix_numsamples,%eax

CalculateLoopCount:
	mov    %eax,mix_count
	mov    40(%ecx),%esi
	mov    $0,%ebp
	mov    %ebp,mix_endflag					
	
	cmpl   $1,56(%ecx)
	jne    samplesleftbackwards
	mov    8(%ebx),%edx
	add    12(%ebx),%edx
	sub    %esi,%edx						
	mov    44(%ecx),%eax
	xor    %ebp,%ebp
	sub    %eax,%ebp

	sbb   $0x0,%edx
	mov    %ebp,%eax

	jmp   samplesleftfinish

samplesleftbackwards:
	mov    40(%ecx),%edx
	mov    44(%ecx),%eax

	sub    $0x0,%eax
	sbb    8(%ebx),%edx

samplesleftfinish:
	cmp    $0x1000000,%edx
	jae    staywithoutputbuffend

	shrd   $0x8,%edx,%eax
	shr    $0x8,%edx

	mov    52(%ecx),%ebp
	mov    48(%ecx),%edi
	shl    $0x18,%ebp
	shr    $0x8,%edi
	and    $0xffffff,%edi
	or     %edi,%ebp
	div    %ebp
	or     %edx,%edx	
	je     dontaddbyte	
	inc    %eax
dontaddbyte:
	cmp    mix_count,%eax
	ja     staywithoutputbuffend
	mov    %eax,mix_count
	mov    $1,%edx
	mov    %edx,mix_endflag
staywithoutputbuffend:
	mov    mix_cptr,%ecx
	
	movl   $0x0,mix_rampspeedleft
	movl   $0x0,mix_rampspeedright

#if defined( VOLUMERAMPING )
	mov    mix_count,%eax
	mov    %eax,mix_count_old
	
	movl   $0x0,mix_rampcount

	cmpl   $0x0,84(%ecx)
	je     volumerampstart

	mov    32(%ecx),%eax
	mov    60(%ecx),%edx
	cmp    %edx,%eax
	jne    volumerampstart
	mov    36(%ecx),%eax
	mov    64(%ecx),%edx
	cmp    %edx,%eax
	jne    volumerampstart
	
	mov    84(%ecx),%eax
	mov    %eax,mix_rampcount
	mov    76(%ecx),%eax
	mov    %eax,mix_rampspeedleft
	mov    80(%ecx),%eax
	mov    %eax,mix_rampspeedright
	
	jmp    novolumerampR

volumerampstart:
	mov    32(%ecx),%eax
	mov    68(%ecx),%edx
	shr    $0x8,%edx
	mov    %eax,60(%ecx)
	sub    %edx,%eax

	cmp    $0x0,%eax
	je     novolumerampL

	mov    %eax,mix_temp1
	fildl  mix_temp1
	fmuls  mix_1over255

	fmuls  _mix_1overvolumerampsteps
	fstps  mix_rampspeedleft
	mov    mix_rampspeedleft,%eax
	mov    %eax,76(%ecx)

	mov    _mix_volumerampsteps,%eax
	mov    %eax,mix_rampcount

novolumerampL:
	mov    36(%ecx),%eax
	mov    72(%ecx),%edx
	shr    $0x8,%edx
	mov    %eax,64(%ecx)
	sub    %edx,%eax

	cmp    $0x0,%eax
	je     novolumerampR

	mov    %eax,mix_temp1
	fildl  mix_temp1
	fmuls  mix_1over255
	fmuls  _mix_1overvolumerampsteps
	fstps  mix_rampspeedright
	mov    mix_rampspeedright,%eax
	mov    %eax,80(%ecx)

	mov    _mix_volumerampsteps,%eax
	mov    %eax,mix_rampcount

novolumerampR:
	mov    mix_rampcount,%eax

	cmp    $0x0,%eax
	jle    volumerampend

	mov    %eax,84(%ecx)

	cmp    %eax,mix_count
	jbe    volumerampend
	mov    %eax,mix_count

volumerampend:
#endif

	mov    mix_cptr,%ecx

	mov    36(%ecx),%eax
	mov    %eax,mix_temp1
	fildl  mix_temp1
	fmuls  mix_1over255
	fstps  mix_rightvol

	mov    32(%ecx),%eax
	mov    %eax,mix_temp1
	fildl  mix_temp1
	fmuls  mix_1over255
	fstps  mix_leftvol

	mov    72(%ecx),%eax
	mov    %eax,mix_temp1
	fildl  mix_temp1
	fmuls  mix_1over256
	fmuls  mix_1over255
	fstps  mix_ramprightvol

	mov    68(%ecx),%eax
	mov    %eax,mix_temp1
	fildl  mix_temp1
	fmuls  mix_1over256
	fmuls  mix_1over255
	fstps  mix_rampleftvol

	mov    mix_cptr,%eax
	mov    48(%eax),%ebx
	mov    52(%eax),%ecx
	mov    44(%eax),%ebp
	mov    40(%eax),%esi
	mov    mix_mixbuffptr,%edi
	cmpl   $1,56(%eax)
	je     NoChangeSpeed
	xor    $0xffffffff,%ebx
	xor    $0xffffffff,%ecx
	add    $0x1,%ebx
	adc    $0x0,%ecx
	
NoChangeSpeed:

	mov    mix_samplebuff,%eax
	shr    %eax
	add    %eax,%esi

	mov    mix_count,%edx

#if defined( VOLUMERAMPING )
	cmpl   $0x0,mix_rampcount
	jne    MixLoopStart16_2
#endif

#if defined( ROLLED )
	jmp     MixLoopStart16
#endif

	shr    %edx
	or     %edx,%edx
	jz     MixLoopStart16	

	shr    %ebp
	add    $0x10,%edi
	fild   0x2(%esi,%esi,1)
	mov    %ebp,mix_temp1
	fild   (%esi,%esi,1)
	fildl  mix_temp1
	add    %ebp,%ebp
	add    %ebx,%ebp
	adc    %ecx,%esi
	fmuls  mix_1over2gig
	fild   0x2(%esi,%esi,1)
	shr    %ebp
	mov    %ebp,mix_temp1
	fildl  mix_temp1
	fild   (%esi,%esi,1)
	fxch   %st(5)
	fsub   %st(4),%st
	shl    %ebp
	fmulp  %st,%st(3)
	fmuls  mix_1over2gig
	fxch   %st(1)
	fsub   %st(4),%st
	add    %ebx,%ebp
	adc    %ecx,%esi
	fmulp  %st,%st(1)
	fxch   %st(1)
	faddp  %st,%st(2)
	fld    %st(1)
	fmuls  mix_leftvol
	fxch   %st(1)
	faddp  %st,%st(3)
	fxch   %st(1)
	jmp    MixLoopUnroll16CoilEntry

# .align 16	

MixLoopUnroll16:
	shr    %ebp
	mov    %ebp,mix_temp1
	fild   0x2(%esi,%esi,1)
	fild   (%esi,%esi,1)
	fildl  mix_temp1
	add    %ebp,%ebp
	add    %ebx,%ebp
	adc    %ecx,%esi
	add    $0x10,%edi
	fmuls  mix_1over2gig
	shr    %ebp
	mov    %ebp,mix_temp1
	fildl  mix_temp1
	fild   0x2(%esi,%esi,1)
	fxch   %st(4)
	fsub   %st(3),%st
	shl    %ebp
	fmulp  %st,%st(2)
	fild   (%esi,%esi,1)
	fxch   %st(1)
	fmuls  mix_1over2gig
	fxch   %st(4)
	fsub   %st(1),%st
	add    %ebx,%ebp
	adc    %ecx,%esi
	fmulp  %st,%st(4)
	fxch   %st(2)
	faddp  %st,%st(1)
	fxch   %st(4)
	fstps  0xffffffe4(%edi)
	fxch   %st(4)
	fstps  0xffffffe8(%edi)
	fld    %st(2)
	fmuls  mix_leftvol
	fxch   %st(1)
	faddp  %st,%st(4)
	fxch   %st(1)
	fstps  0xffffffec(%edi)
	fxch   %st(1)

MixLoopUnroll16CoilEntry:
	fmuls  mix_rightvol
	fxch   %st(2)
	fld    %st(0)
	fmuls  mix_leftvol
	fxch   %st(1)
	fmuls  mix_rightvol
	fxch   %st(2)
	fadds  0xfffffff0(%edi)
	fxch   %st(3)
	fadds  0xfffffff4(%edi)
	fxch   %st(1)
	fadds  0xfffffff8(%edi)
	fxch   %st(3)
	fstps  0xfffffff0(%edi)
	fxch   %st(1)
	fadds  0xfffffffc(%edi)

	dec    %edx
	jne    MixLoopUnroll16

	fxch   %st(1)
	fstps  0xfffffff4(%edi)
	fxch   %st(1)
	fstps  0xfffffff8(%edi)
	fstps  0xfffffffc(%edi)

	#= MIX 16BIT, ROLLED ==================================================================
MixLoopStart16:
	mov    mix_count,%edx
#if ! defined( ROLLED )
	and    $0x01,%edx
#endif

#if defined( VOLUMERAMPING )
MixLoopStart16_2:
#endif
	or     %edx,%edx
	je     MixLoopEnd16

#if defined( VOLUMERAMPING )
	flds   mix_rampspeedleft
	flds   mix_rampspeedright
	flds   mix_rampleftvol
	flds   mix_ramprightvol
#else
	fldz
	fldz
	flds   mix_leftvol
	flds   mix_rightvol
#endif

	jmp    MixLoop16

#	.align 16
MixLoop16:
	shr    %ebp
	add    $0x8,%edi
	fild   0x2(%esi,%esi,1)
	mov    %ebp,mix_temp1
	fild   (%esi,%esi,1)
	fildl  mix_temp1
	shl    %ebp
	add    %ebx,%ebp
	adc    %ecx,%esi
	fmuls  mix_1over2gig
	fxch   %st(2)
	fsub   %st(1),%st
	fmulp  %st,%st(2)
	faddp  %st,%st(1)
	fld    %st(0)
	fmul   %st(3),%st
	fxch   %st(3)
	fadd   %st(5),%st
	fxch   %st(1)
	fmul   %st(2),%st
	fxch   %st(2)
	fadd   %st(4),%st
	fxch   %st(3)
	fadds  0xfffffff8(%edi)
	fxch   %st(2)
	fadds  0xfffffffc(%edi)
	fxch   %st(2)
	fstps  0xfffffff8(%edi)
	fxch   %st(1)
	fstps  0xfffffffc(%edi)
	fxch   %st(1)

	dec    %edx
	jne    MixLoop16

	fxch   %st(2)
	fstps  mix_rampspeedright
	fxch   %st(2)
	fstps  mix_rampspeedleft

	fmuls  mix_255
	fmuls  mix_256
	fxch   %st(1)
	fmuls  mix_255
	fmuls  mix_256

	xor    %eax,%eax
	fistpl mix_rampleftvol
	fistpl mix_ramprightvol

MixLoopEnd16:
	mov    mix_samplebuff,%eax
	shr    %eax
	sub    %eax,%esi

#if defined( VOLUMERAMPING )
	cmpl   $0x0,mix_rampcount
	je     DoOutputbuffEnd			

	mov    mix_sptr,%ebx			
	mov    mix_cptr,%ecx			
	
	mov    mix_rampleftvol,%eax
	mov    %eax,68(%ecx)
	mov    mix_ramprightvol,%eax
	mov    %eax,72(%ecx)
	
	mov    mix_count,%eax
	mov    mix_rampcount,%edx

	sub    %eax,%edx

	movl   $0x0,mix_rampspeedleft	
	movl   $0x0,mix_rampspeedright	
	mov    %edx,mix_rampcount
	mov    %edx,84(%ecx)
	cmp    $0x0,%edx
	jne    DoOutputbuffEnd

	movl   $0x0,76(%ecx)
	movl   $0x0,80(%ecx)

	mov    32(%ecx),%edx
	shl    $0x8,%edx
	mov    %edx,68(%ecx)
	mov    36(%ecx),%edx
	shl    $0x8,%edx
	mov    %edx,72(%ecx)

	cmpl   %eax,mix_count_old	
	je     DoOutputbuffEnd

	mov    %esi,40(%ecx)
	mov    %ebp,44(%ecx)

	mov    mix_mixbuffend,%eax	
	sub    %edi,%eax
	shr    $0x3,%eax			
	mov    %edi,mix_mixbuffptr	
	
	cmp    $0x0,%eax			
	jne    CalculateLoopCount

DoOutputbuffEnd:
#endif

	cmpl   $0,mix_endflag
	je     FinishUpChannel

	mov    mix_sptr,%ebx	
	mov    mix_cptr,%ecx	

	mov    29(%ebx),%dl
	
	test   $2,%dl
	je     CheckBidiLoop
	
	mov    8(%ebx),%eax
	add    12(%ebx),%eax
rewindsample:
	sub    12(%ebx),%esi
	cmp    %eax,%esi
	jae    rewindsample

	mov    %esi,40(%ecx)
	mov    %ebp,44(%ecx)
	mov    mix_mixbuffend,%eax 			
	sub    %edi,%eax
	shr    $0x3,%eax					
	mov    %edi,mix_mixbuffptr			

	cmp    $0x0,%eax
	je     FinishUpChannel
	
	jmp    CalculateLoopCount

CheckBidiLoop:
	test   $4,%dl
	je     NoLoop
	cmpl   $1,56(%ecx)
	je     BidiForward

BidiBackwards:
	mov    8(%ebx),%eax
	dec    %eax
	mov    $0xffffffff,%edx
	sub    %ebp,%edx
	sbb    %esi,%eax

	mov    %eax,%esi
	mov    %edx,%ebp			
	mov    8(%ebx),%eax
	mov    $0x0,%edx
	add    %edx,%ebp
	adc    %eax,%esi			
	
	movl   $1,56(%ecx)
	mov    8(%ebx),%eax
	add    12(%ebx),%eax
	cmp    %eax,%esi
	jge    BidiForward

	jmp    BidiFinish

BidiForward:
	mov    8(%ebx),%eax
	add    12(%ebx),%eax
	mov    $0x0,%edx
	sub    %ebp,%edx
	sbb    %esi,%eax
	mov    %eax,%esi
	mov    %edx,%ebp							

	mov    8(%ebx),%eax
	add    12(%ebx),%eax
	dec    %eax
	mov    $0xffffffff,%edx
	add    %edx,%ebp
	adc    %eax,%esi

	movl   $2,56(%ecx)

	cmp    8(%ebx),%esi
	jl     BidiBackwards

BidiFinish:

	mov    %esi,40(%ecx)
	mov    %ebp,44(%ecx)

	mov    mix_mixbuffend,%eax		
	sub    %edi,%eax
	shr    $0x03,%eax				
	mov    %edi,mix_mixbuffptr		

	cmp    $0x0,%eax
	je     FinishUpChannel

	jmp    CalculateLoopCount

NoLoop:
	xor    %ebp,%ebp
	xor    %esi,%esi
	mov    %esi,28(%ecx)	

FinishUpChannel:
	mov    (mix_cptr),%ecx
	mov    %ebp,44(%ecx)
	mov    %esi,40(%ecx)		
	
MixExit:
	pop    %ebp
	ret
