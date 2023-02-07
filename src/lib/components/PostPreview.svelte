<script lang="ts">
	import type { PostInfo } from '$lib/types/post';
	export let post: PostInfo;
</script>

<section>
	<h2 class="text-xl font-medium text-gray-800 dark:text-gray-200 sm:text-2xl">{post.title}</h2>
	<p class="text-sm text-gray-500 dark:text-gray-400">{post.date}</p>
	<p class="mt-2">{post.excerpt}</p>
	<a
		class="mt-4 inline-flex rounded-md border-2 border-green-700 py-2 px-3 text-sm font-bold text-green-700 hover:bg-green-700 hover:text-gray-50 focus:outline-none
		focus:ring focus:ring-green-300 active:border-green-800 active:bg-green-800 active:text-gray-50 dark:border-green-500 dark:text-green-500 
		dark:hover:bg-green-500 dark:hover:text-gray-900 dark:focus:ring-green-300 dark:active:border-green-600 dark:active:bg-green-600 dark:active:text-gray-900"
		href={post.slug}>Read more →</a
	>
</section>
