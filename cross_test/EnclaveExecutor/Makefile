include ../buildenv.mk

ifneq ($(SGX_MODE), HW)
        Trts_Library_Name := sgx_trts_sim
        Service_Library_Name := sgx_tservice_sim
else
        Trts_Library_Name := sgx_trts
        Service_Library_Name := sgx_tservice
endif

Crypto_Library_Name := sgx_tcrypto

ENCLAVE_NAME := libenclave_executor.so
SIGNED_ENCLAVE_NAME := libenclave_executor.signed.so

$(SIGNED_ENCLAVE_NAME) : $(ENCLAVE_NAME)
	@$(SGX_ENCLAVE_SIGNER) sign -key EnclaveExecutor_private.pem -enclave $(ENCLAVE_NAME) -out $@ -config EnclaveExecutor.config.xml
	@cp $(SIGNED_ENCLAVE_NAME) $(TOPDIR)/$(OUTDIR)/
	@echo "SIGN =>  $@"

# Enable the security flags
Enclave_Security_Link_Flags := -Wl,-z,relro,-z,now,-z,noexecstack

# To generate a proper enclave, it is recommended to follow below guideline to link the trusted libraries:
#    1. Link sgx_trts with the `--whole-archive' and `--no-whole-archive' options,
#       so that the whole content of trts is included in the enclave.
#    2. For other libraries, you just need to pull the required symbols.
#       Use `--start-group' and `--end-group' to link these libraries.
# Do NOT move the libraries linked with `--start-group' and `--end-group' within `--whole-archive' and `--no-whole-archive' options.
# Otherwise, you may get some undesirable errors.
ENCLAVE_LINK_FLAGS := $(Enclave_Security_Link_Flags) \
     -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
             -Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
             -Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
             -Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
             -Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
             -Wl,--defsym,__ImageBase=0 -Wl,--gc-sections   \
             -Wl,--version-script=EnclaveExecutor.lds

SGX_COMMON_FLAGS += -Wall -Wextra -Winit-self -Wpointer-arith -Wreturn-type \
                    -Waddress -Wsequence-point -Wformat-security \
                    -Wmissing-include-dirs -Wfloat-equal -Wundef -Wshadow \
                    -Wcast-align -Wconversion -Wredundant-decls

SGX_COMMON_CFLAGS := $(SGX_COMMON_FLAGS) -Wjump-misses-init -Wstrict-prototypes -Wunsuffixed-float-constants
SGX_COMMON_CXXFLAGS := $(SGX_COMMON_FLAGS) -Wnon-virtual-dtor -std=c++11

Enclave_Include_Paths := -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx -I../Include

Enclave_C_Flags := $(Enclave_Include_Paths) -nostdinc -fvisibility=hidden -fpie -ffunction-sections -fdata-sections
CC_BELOW_4_9 := $(shell expr "`$(CC) -dumpversion`" \< "4.9")
ifeq ($(CC_BELOW_4_9), 1)
        Enclave_C_Flags += -fstack-protector
else
        Enclave_C_Flags += -fstack-protector-strong
endif
Enclave_Cpp_Flags := $(Enclave_C_Flags) -nostdinc++

Enclave_Cpp_Files := $(wildcard *.cpp)
ENCLAVE_CPP_OBJECTS := $(Enclave_Cpp_Files:.cpp=.o)

.PHONY = all clean target

target = $(SIGNED_ENCLAVE_NAME)

all: 
	@make target

clean:
	@rm -f  $(ENCLAVE_NAME) $(SIGNED_ENCLAVE_NAME) *.o *_t.c *_t.h *_u.c *_u.h

$(ENCLAVE_NAME):EnclaveExecutor_t.o $(ENCLAVE_CPP_OBJECTS)
	@$(CXX) $^ -o $@ $(ENCLAVE_LINK_FLAGS)
	@echo "LINK => $@"

######## Enclave Objects ########

EnclaveExecutor_t.h: $(SGX_EDGER8R) EnclaveExecutor.edl
	@$(SGX_EDGER8R) --trusted EnclaveExecutor.edl --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

EnclaveExecutor_t.c: EnclaveExecutor_t.h

EnclaveExecutor_t.o: EnclaveExecutor_t.c
	@$(CC) $(SGX_COMMON_CFLAGS) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

%.o: %.cpp EnclaveExecutor_t.h
	@$(CXX) $(SGX_COMMON_CXXFLAGS) $(Enclave_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

