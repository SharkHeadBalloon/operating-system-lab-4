#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_TOKEN_LEN 64
#define MAX_CMDLN_LEN 256

typedef unsigned short Status;

Status mycopy(const char*, const char*);
Status file_copy(const char*, const char*, const char*);
Status file_copy(const char*, const char*, const char*, const char*);
Status get_token(const char*, char*, int, unsigned char);
Status dir_cat(char *, const char*, const char*);
Status dir_preprocess(char*, char*, const char*);

int main()
{
	// Get track of the current directory. This variable should be only 
	// editted once
	char working_path[MAX_TOKEN_LEN] = "";
	if(!getcwd(working_path, MAX_TOKEN_LEN))
	{
		printf("Get working path fail!\n");
		return -1;
	}

	Status running_signal = 1;
	char info[MAX_TOKEN_LEN] = "Operating_system_test_console> ";
	while (running_signal)
	{
		fputs(info, stdout);
		char command_line[MAX_CMDLN_LEN] = "";
		fgets(command_line, MAX_CMDLN_LEN, stdin);
		running_signal = mycopy(command_line, working_path);
		if (running_signal == -1)
		{
			printf("Error. Try again\n");
		}
			
	}
}

Status mycopy(const char* command_line, const char* current_dir)
{
	char cmd[MAX_TOKEN_LEN] = "";
	char dir_src[MAX_TOKEN_LEN] = "";
	char dir_dest[MAX_TOKEN_LEN] = "";
	char file_src[MAX_TOKEN_LEN] = "";
	char file_dest[MAX_TOKEN_LEN] = "";
	char line_src[MAX_TOKEN_LEN] = "";
	char line_dest[MAX_TOKEN_LEN] = "";
	char tail[MAX_TOKEN_LEN] = "";
	
	// decode the command line
	if (get_token(command_line, cmd, 0, ' ') <= 0)
	{// deal with enter
		return 1;
	}

	if (strncmp(cmd, "quit", MAX_TOKEN_LEN) == 0)
	{// time to quit
		return 0;
	}
	else if (strncmp(cmd, "mycopy", MAX_TOKEN_LEN) != 0)
	{// illegal command type
		printf("Unknown command %s\n", cmd);
		return -1;
	}

	if (get_token(command_line, line_src, 1, ' ') <= 0
	 || get_token(command_line, line_dest, 2, ' ') <= 0
	 || get_token(command_line, tail, 3, ' ') > 0)
	{// illegal command operand number
		printf("Please check your input format.\n");
		return -1;
	}

	strncpy(dir_src, current_dir, MAX_TOKEN_LEN);
	strncpy(dir_dest, current_dir, MAX_TOKEN_LEN);
	
	dir_preprocess(dir_src, file_src, line_src);
	dir_preprocess(dir_dest, file_dest, line_dest);

	file_copy(file_src, file_dest, dir_src, dir_dest);

	return 1;
}

Status file_copy(const char* file_src, const char* file_dest, 
				 const char* dir_src, const char* dir_dest)
{//file_src is the d_name of the file, dir_src is the directory of it. Same for dest.
	DIR * d_src;
	d_src = opendir(dir_src);
	if (d_src == NULL)
	{
		printf("Open source directory fail\n");
		return -1;
	}
	struct dirent* drt;
	Status src_found = 0;
	
	for (drt = readdir(d_src); drt != NULL; drt = readdir(d_src))
	{
		if (strncmp(drt->d_name, file_src, MAX_TOKEN_LEN) == 0)
		{// 找到file_src文件，结束就跳出循环进行复制操作。
			src_found = 1;
			break;
		}
	}

	if (!src_found)
	{
		printf("Source file not found!\n");
		return -1;
	}

	if (drt->d_type == DT_DIR)
	{
		char new_dir_src[MAX_TOKEN_LEN];
		dir_cat(new_dir_src, dir_src, file_src);
		char new_dir_dest[MAX_TOKEN_LEN];
		dir_cat(new_dir_dest, dir_dest, file_dest);

		if (mkdir(new_dir_dest, 0755) == -1)
		{
			printf("mkdir fail.\n");
			return -1;
		}
		DIR *new_d_src, *d_dest;
		
		new_d_src = opendir(new_dir_src);
		if (new_d_src == NULL)
		{
			printf("Open new source directory fail.\n");
			return -1;
		}

		struct dirent* new_drt;
		for (new_drt = readdir(new_d_src); new_drt != NULL; new_drt = readdir(new_d_src))
		{
			if (strncmp(new_drt->d_name, "..", 4) == 0 ||
				strncmp(new_drt->d_name, ".", 4) == 0)
				continue;
			
			file_copy(new_drt->d_name, new_drt->d_name, new_dir_src, new_dir_dest);
		}
		closedir(new_d_src);
	}
	else if (drt->d_type == DT_LNK)
	{
		char target[MAX_TOKEN_LEN] = "";
		char abs_file_src[MAX_TOKEN_LEN] = "";
		dir_cat(abs_file_src, dir_src, drt->d_name);
		if (readlink(abs_file_src, target, MAX_TOKEN_LEN) == -1)
		{
			printf("Read soft link error.\n");
			return -1;
		}
		char abs_file_dest[MAX_TOKEN_LEN] = "";
		dir_cat(abs_file_dest, dir_dest, file_dest);
		if (symlink(target, abs_file_dest) == -1)
		{
			printf("Create soft link error.\n");
			return -1;
		}
	}
	else
	{
		// 打开源文件，注意要将源文件转换成绝对路径
		char abs_file_src[MAX_TOKEN_LEN] = "";
		dir_cat(abs_file_src, dir_src, file_src);
		int fd_src = open(abs_file_src, O_RDONLY);
		if (fd_src == -1)
		{
			printf("Open source error.\n");
			return -1;
		}

		// 打开目标文件，注意要将源文件转换成绝对路径
		char abs_file_dest[MAX_TOKEN_LEN] = "";
		dir_cat(abs_file_dest, dir_dest, file_dest);
		int fd_dest = open(abs_file_dest, O_WRONLY|O_CREAT|O_APPEND, 0766);
		if (fd_dest == -1)
		{
			printf("Open destnation error.\n");
			return -1;
		}
		char* buffer = (char*)malloc(2 * sizeof(char));
		int k;
		do
		{
			memset(buffer, 0, sizeof(buffer));
			k = read(fd_src, buffer, 1);
			if (k <= 0)
				break;
			write(fd_dest, buffer, 1);
		} while (k > 0);
		
		free(buffer);
		close(fd_dest);
		close(fd_src);
	}

	closedir(d_src);

	return 0;
}

Status get_token(const char* src, char* buff, int count, unsigned char end_sym)
{// get the count-th token in src and store it in buff
// return lenth of buff 
	if (count < 0)
	{
		printf("Internal error\n");
		system("exit");
	}
	int src_iter = 0;
	int buff_iter = 0;
	// jump all blanks ahead
	while(src[src_iter] == end_sym)
		src_iter++;
	for (src_iter; src[src_iter] != '\n' && src[src_iter] != '\0'; src_iter++)
	{
		if (src[src_iter] == end_sym)
		{// encounter blank, jump all of them, get next token
			while (src[src_iter] == end_sym)
				src_iter ++;
			src_iter --;
			count --;
			continue;
		}
		if (count == 0)
		{// come to the current token, record it
			buff[buff_iter++] = src[src_iter];
		}
		else if(count == -1)
		{
			break;
		}
	}
	buff[buff_iter] = '\0';
	return strlen(buff);
}

Status dir_preprocess(char* dir, char* file, const char* line)
{// transfer input file name into relative file name and directory to it.
	if (line[0] == '/')
		return -1;
	int iter = strlen(line) - 1;
	
	while(line[iter] != '/' && --iter > 0);

	if (iter)
	{// 如果存在'/', 在目录后添加。。。, 文件名注意要除掉'/'
		strncpy(file, line + iter + 1, MAX_TOKEN_LEN);
		strncat(dir, "/", 2);
		strncat(dir, line, iter);
	}
	else
	{
		strncpy(file, line, MAX_TOKEN_LEN);
	}


	return 1;

}

Status dir_cat(char * res, const char * head, const char* tail)
{
	strncpy(res, head, MAX_TOKEN_LEN);
	if (res[strlen(res) - 1] != '/')
		strncat(res, "/", 2);
	strncat(res, tail, MAX_TOKEN_LEN);

	return 1;
}

