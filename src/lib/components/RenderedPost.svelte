<script lang="ts">
	import type Post from '$lib/types/post';
	import type { ComponentType } from 'svelte';
	import Header from './Header.svelte';

	export let Content: ComponentType;
	export let meta: Post;
</script>

<article class="mx-auto max-w-3xl">
	<h1 class="text-2xl font-medium text-gray-900 dark:text-gray-50 sm:text-3xl">{meta.title}</h1>
	<h2 class="mt-1 text-sm sm:text-base text-gray-500 dark:text-gray-400">{meta.date}</h2>

	<div class="mt-8 prose prose-gray dark:prose-invert max-w-none dark:prose-img:brightness-90 prose-img:rounded-lg prose-img:mx-auto
	prose-a:no-underline prose-a:text-green-700 hover:prose-a:text-green-800 dark:prose-a:text-green-500 dark:hover:prose-a:text-green-600
	prose-code:text-purple-700 dark:prose-code:text-orange-500 prose-code:font-mono prose-code:not-italic prose-code:before:contents prose-code:after:contents prose-code:font-light">
		<svelte:component this={Content} />
	</div>
</article>
