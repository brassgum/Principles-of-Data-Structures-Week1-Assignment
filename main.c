#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEFAULT_BUFFER_SIZE (5)

typedef struct list_s
{
	struct list_s* next;
	int* buffer;
	size_t buffer_size;
	size_t relative_pos;
} list_t;

void print_list(const list_t* const list)
{
	const list_t* current = list;
	while (current != NULL)
	{
		for (size_t i = 0; i < current->relative_pos; i++)
		{
			printf("%d, ", current->buffer[i]);
		}
		current = current->next;
	}
	printf("\n");
}

void append(list_t* const out_list, int value)
{
	list_t* current = out_list;

	while (current != NULL)
	{
		if (current->relative_pos < current->buffer_size)
		{
			current->buffer[current->relative_pos++] = value;
			return;
		}
		current = current->next;
	}
	// Craete new node
	current = out_list;
	assert(current != NULL);

	while (current->next != NULL)
	{
		current = current->next;
	}

	const size_t NEW_BUFFER_SIZE = DEFAULT_BUFFER_SIZE;
	const size_t NEW_NODE_SIZE = sizeof(list_t) + sizeof(int) * NEW_BUFFER_SIZE;

	current->next = malloc(NEW_NODE_SIZE);
	assert(current->next != NULL);


	current = current->next;
	current->buffer = (int*)(current + 1);
	current->next = NULL;
	current->buffer_size = NEW_BUFFER_SIZE;
	current->relative_pos = 0;
	current->buffer[current->relative_pos++] = value;
	return;
}

void change_at(list_t* const out_list, int new_value, size_t pos)
{
	list_t* current = out_list;
	while (current != NULL && current->buffer_size <= pos)
	{
		pos -= current->buffer_size;
		current = current->next;
	}
	if (current == NULL) { return; }
	current->buffer[pos] = new_value;
}

void add_at(list_t* const out_list, int value, size_t pos)
{
	list_t* current = out_list;
	list_t* current_bak = current;
	while (current != NULL && pos >= current->buffer_size)
	{
		pos -= current->relative_pos;
		current_bak = current = current->next;
	}

	if (current == NULL)
	{
		if (pos == 0)
		{
			append(out_list, value);
			return;
		}
		printf("Invalid position\n");
		return;
	}
	while (current->next != NULL)
	{
		current = current->next;
	}

	append(current, current->buffer[current->relative_pos - 1]);

	current = current_bak;
	size_t size_sum = 0;
	while (current != NULL)
	{
		size_sum += current->relative_pos;
		current = current->next;
	}

	int* const buffer = malloc(sizeof(int) * size_sum);
	size_t idx = 0;
	assert(buffer != NULL);
	current = current_bak;

	for (size_t i = pos; i < current->relative_pos; i++)
	{
		buffer[idx++] = current->buffer[i];
	}
	current = current->next;
	while (current != NULL)
	{
		for (size_t i = 0; i < current->relative_pos && idx < size_sum; i++)
		{
			buffer[idx++] = current->buffer[i];
		}
		current = current->next;
	}

	idx = 0;
	current = current_bak;
	for (size_t i = pos; i < current->relative_pos - 1; i++)
	{
		current->buffer[i + 1] = buffer[idx++];
	}
	int next_num = buffer[idx++];
	current = current->next;
	while (current != NULL)
	{
		current->buffer[0] = next_num;
		for (size_t i = 0; i < current->relative_pos - 1; i++)
		{
			current->buffer[i + 1] = buffer[idx++];
		}
		next_num = buffer[idx++];
		current = current->next;
	}
	free(buffer);
	change_at(current_bak, value, pos);
}

void delete_at(list_t* const out_list, size_t pos)
{
	list_t* current = out_list;
	while (current != NULL && current->buffer_size < pos)
	{
		pos -= current->buffer_size;
		current = current->next;
	}
	if (current == NULL)
	{
		return;
	}
	const size_t LENGTH = current->relative_pos - 1;
	for (size_t i = pos; i < LENGTH; i++)
	{
		current->buffer[i] = current->buffer[i + 1];
	}
	while (current != NULL)
	{
		if (current->next != NULL && current->buffer_size == current->relative_pos && current->next->relative_pos != 0)
		{
			current->buffer[current->buffer_size - 1] = current->next->buffer[0];
		}
		if (current->next == NULL)
		{
			current->relative_pos--;
			return;
		}
		current = current->next;
		for (size_t i = 0; i < current->relative_pos - 1; i++)
		{
			current->buffer[i] = current->buffer[i + 1];
		}
		if (current->next == NULL || (current->next != NULL && current->next->relative_pos == 0))
		{
			current->relative_pos--;
			return;
		}
	}

}

void shutdown(list_t* list)
{
	// release list
	list_t* temp;
	while (list != NULL)
	{
		temp = list->next;
		free(list);
		list = temp;
	}
}

int main(void)
{
	srand(1);
	size_t input_size;
	printf("숫자의 개수를 입력하시오: ");

	// get_input
	scanf_s("%llu", &input_size);
	list_t* const input_list = malloc(sizeof(list_t) + sizeof(int) * input_size);
	assert(input_list != NULL);

	input_list->buffer = (int*)(input_list + 1);
	input_list->buffer_size = input_list->relative_pos = input_size;
	input_list->next = NULL;
	// get input
	for (size_t i = 0; i < input_list->relative_pos; i++)
	{
		input_list->buffer[i] = rand() % 100;
	}
	print_list(input_list);
	// menu
	unsigned int menu_input;
	int value_input;
	size_t pos_input;
	while (1)
	{
		printf("(1) 끝에 값 추가\n(2) pos번째에 값 추가\n(3) pos번째에 값 삭제\n(4) pos번째 값 변경\n(5) 종료\t메뉴선택 => ");
		scanf_s("%u", &menu_input);
		switch (menu_input)
		{
		case 1:
			printf("끝에 추가할 값: ");
			scanf_s("%d", &value_input);
			printf("값 %d를 리스트 맨 끝에 추가합니다.\n", value_input);
			append(input_list, value_input);
			break;
		case 2:
			printf("추가할 위치와 값: ");
			scanf_s("%llu %d", &pos_input, &value_input);
			add_at(input_list, value_input, pos_input);
			break;
		case 3:
			printf("삭제할 위치: ");
			scanf_s("%llu", &pos_input);
			delete_at(input_list, pos_input);
			break;
		case 4:
			printf("변경할 위치와 값: ");
			scanf_s("%llu %d", &pos_input, &value_input);
			change_at(input_list, value_input, pos_input);
			break;	
		case 5:
			printf("안녕히 가세요.\n");
			shutdown(input_list);
			return 0;
		default:
			printf("잘못된 값이 입력되었습니다. 메뉴에 맞는 정수를 입력해주세요.\n");
			goto lb_invalid_menu_input;
		}
		print_list(input_list);
	lb_invalid_menu_input:;
	}

}
