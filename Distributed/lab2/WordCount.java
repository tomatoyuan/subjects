package experiment2;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;

public class WordCount {

	/*
	 * @Object 存储<key, value>对的 key 值，此处为文本数据起始位置的偏移量
	 * 
	 * @Text 存储<key, value>对的 value，是具体的文本数据，查API知是String的封装
	 * 
	 * @Text 输出<key, value>对的 key 值，此处为每一个单词，例如`name`
	 * 
	 * @IntWritable 输出<key, value>对的 value，这里是 1，IntWritable是对Integer的封装
	 */
	public static class TokenizerMapper extends Mapper<Object, Text, Text, IntWritable> {

		private final static IntWritable one = new IntWritable(1);
		private Text word = new Text();

		/*
		 * @key 输入在源数据的偏移量
		 * 
		 * @value 一段字符串
		 * 
		 * @context 暂存 map 处理后的结果
		 */
		@Override
		public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
			// 将读入的每一行转化为字符串
			StringTokenizer itr = new StringTokenizer(value.toString());
			// 将字符串拆分为单词，并打包成<word, 1>暂存在context中
			while (itr.hasMoreTokens()) {
				word.set(itr.nextToken());
				context.write(word, one);
			}
		}
	}

	/*
	 * @Text 输入单词名
	 * 
	 * @IntWritable 输入shuffle和sort操作之后的列表<1,1,1,1...>
	 * 
	 * @Text 输出单词名
	 * 
	 * @IntWritable 输出每个单词对应的出现次数
	 */
	public static class IntSumReducer extends Reducer<Text, IntWritable, Text, IntWritable> {

		private IntWritable result = new IntWritable();

		/*
		 * @key 输入单词名
		 * 
		 * @values 输入单词列表 <1,1,1...>
		 * 
		 * @context 暂存输出<key, value>对
		 */
		@Override
		public void reduce(Text key, Iterable<IntWritable> values, Context context)
				throws IOException, InterruptedException {
			int sum = 0;
			// 统计单词出现的次数
			for (IntWritable val : values) {
				sum += val.get();
			}
			result.set(sum);
			context.write(key, result);
		}
	}

	public static void main(String[] args) throws Exception {
		Configuration conf = new Configuration();

//	    String[] ioPath = new String[2];
		/* input flie */
//	    ioPath[0] = "E:\\ILOVEHIT\\THIRD\\subjects\\Distributed\\lab\\lab2\\data\\wordCount.txt";
//	    ioPath[0] = "/user/dfstest/wordCount.txt";
		/* result folder */
//	    ioPath[1] = "E:\\ILOVEHIT\\THIRD\\subjects\\Distributed\\lab\\lab2\\data\\result"; 
//	    ioPath[1] = "/user/dfstest/result";

		/* delete output folder if exists */
		Path outputPath = new Path(args[1]);
		outputPath.getFileSystem(conf).delete(outputPath, true);

		String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
		if (otherArgs.length < 2) {
			System.err.println("Usage: wordcount <in> [<in>...] <out>");
			System.exit(2);
		}
		Job job = Job.getInstance(conf, "word count");
		job.setJarByClass(WordCount.class);
		job.setMapperClass(TokenizerMapper.class);
		job.setCombinerClass(IntSumReducer.class);
		job.setReducerClass(IntSumReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(IntWritable.class);
		for (int i = 0; i < otherArgs.length - 1; ++i) {
			FileInputFormat.addInputPath(job, new Path(otherArgs[i]));
		}
		FileOutputFormat.setOutputPath(job, new Path(otherArgs[otherArgs.length - 1]));
		System.exit(job.waitForCompletion(true) ? 0 : 1);
	}
}