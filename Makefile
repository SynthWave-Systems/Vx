CC := gcc
CFLAGS := -s -Wextra -Werror 

TARGET := vx
SRCDIRS := src
INCDIR := inc
OBJDIR := obj
DEPDIR := $(OBJDIR)/deps

GREEN := \033[0;32m
RED := \033[0;31m
NC := \033[0m

SRCS := $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst %.c,$(DEPDIR)/%.d,$(SRCS))
INCFLAGS := $(addprefix -I,$(shell find $(INCDIR) -type d))

DEPFLAGS = -MMD -MF $(@:$(OBJDIR)/%.o=$(DEPDIR)/%.d)


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "$(GREEN)[✓]$(NC) Linking objects to create $(TARGET)..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
	@echo "$(GREEN)[✓]$(NC) Build successful!"


$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D) $(DEPDIR)/$(dir $<)
	@$(CC) $(CFLAGS) $(INCFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)


clean:
	@echo "$(RED)[-]$(NC) Cleaning up..."
	@rm -rf $(OBJDIR)
	@echo "$(RED)[-]$(NC) Cleanup complete."
