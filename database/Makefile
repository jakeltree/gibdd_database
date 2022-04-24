DEPDIR := .deps
CC := g++
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.cc = $(CC) $(DEPFLAGS) -c

%.o : %.cc $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.cc=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))
