CC := gcc
CFLAGS := -s -Wextra -Werror 
TARGET := vx
SRCDIRS := src
INCDIR := inc
OBJDIR := obj
DEPDIR := $(OBJDIR)/deps

SRCS := $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst %.c,$(DEPDIR)/%.d,$(SRCS))

INCFLAGS := $(addprefix -I,$(shell find $(INCDIR) -type d))
DEPFLAGS = -MMD -MF $(@:$(OBJDIR)/%.o=$(DEPDIR)/%.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D) $(DEPDIR)/$(dir $<)
	$(CC) $(CFLAGS) $(INCFLAGS) $(DEPFLAGS) -c $< -o $@


-include $(DEPS)

clean:
	rm -rf $(OBJDIR) $(TARGET)