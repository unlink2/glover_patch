glover_patch:
	mkdir -p bin/
	mkdir -p rom/
	bass payload.s -sym ./bin/payload.sym
	python3 glovepatch.py ./rom/glover_base_ntsc.z64 ./bin/payload.bin ./rom/glover_patched.z64
