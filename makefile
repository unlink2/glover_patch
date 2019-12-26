glover_patch:
	mkdir -p bin/
	mkdir -p rom/
	bass payload.s
	./glovepatch.py ./rom/glover_base_ntsc.z64 ./bin/payload.bin ./rom/glover_patched.z64
